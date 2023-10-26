#pragma once
#include <memory>
#include <vector>
#include "geometry/Bounds.h"

class Renderer;

namespace TinyEngine
{
    class Scene;
    class BVHAccel;

    class BVHNode
    {
        friend class BVHAccel;
        public:
            BVHNode() {}
            ~BVHNode() {}
            
            Bounds GetAABB() const { return bounds; }

            std::shared_ptr<BVHNode> left{nullptr};
            std::shared_ptr<BVHNode> right{nullptr};
            Renderer * object{nullptr};
            Bounds bounds;
    };

    class BVHAccel
    {
    public:
        enum class SplitMethod { NAIVE = 0, SAH = 1 };
        static std::shared_ptr<BVHNode> Build(const Scene& scene, SplitMethod splitMethod = SplitMethod::NAIVE);
        
    private:
        static std::shared_ptr<BVHNode> recursiveBuild(std::vector<Renderer*>& objects, SplitMethod splitMethod);
    };
}