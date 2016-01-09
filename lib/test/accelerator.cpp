#include <iostream>
#include <chrono>

#include <catch/catch.hpp>

#include "ao/tree/tree.hpp"
#include "ao/tree/store.hpp"

#include "ao/render/heightmap.hpp"
#include "ao/gl/accelerator.hpp"
#include "ao/gl/texture.hpp"

static std::pair<DepthImage, NormalImage> RENDER(Tree* t, const Region& r)
{
    GLFWwindow* window = makeContext();
    Accelerator a(t);
    auto out = a.Render(r);
    glfwDestroyWindow(window);
    return out;
}

TEST_CASE("Partial rendering (GPU)")
{
    Store s;
    Tree t(&s, s.constant(-1));

    GLFWwindow* window = makeContext();
    Accelerator a(&t);

    Region r({-1, 1}, {-1, 1}, {-1, 1}, 5);

    GLuint depth, norm;
    glGenTextures(1, &depth);
    glGenTextures(1, &norm);

    a.init(r, depth, norm);

    const double i = -std::numeric_limits<double>::infinity();

    {   // Default depth
        auto d = fromDepthTexture(depth, r);
        CAPTURE(d);
        REQUIRE((d == -std::numeric_limits<double>::infinity()).all());
    }

    {   // Rendering partially over
        Region sub = r.split().first;
        a.RenderSubregion(sub);
        a.finish();

        auto d = fromDepthTexture(depth, r);
        CAPTURE(d);

        DepthImage comp(10, 10);
        comp <<
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i;

        auto diff = d - comp;
        CAPTURE(diff);
        REQUIRE((diff.abs() < 1e-6 || diff != diff).all());
    }

    {   // Rendering over on the other axis
        Region sub = r.split().second.split().first;
        a.RenderSubregion(sub);
        a.finish();

        auto d = fromDepthTexture(depth, r);
        CAPTURE(d);

        DepthImage comp(10, 10);
        comp <<
            0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,
            0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,
            0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,
            0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,
            0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,0.9,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i,
            0.9,0.9,0.9,0.9,0.9,  i,  i,  i,  i,  i;

        auto diff = d - comp;
        CAPTURE(diff);
        REQUIRE((diff.abs() < 1e-6 || diff != diff).all());
    }

    glDeleteTextures(1, &depth);
    glDeleteTextures(1, &norm);
    glfwDestroyWindow(window);
}

#define DESCRIPTION "(GPU)"
#define EPSILON 1e-6
#include "render.ipp"