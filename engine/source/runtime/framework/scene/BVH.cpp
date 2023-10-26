#include "BVH.h"
#include <algorithm>
#include <iostream>
#include <cassert>
#include "object/GameObject.h"
#include "scene/Scene.h"

namespace TinyEngine
{
    inline void Partition_NAIVE(std::vector<Renderer *>& objects, 
            std::vector<Renderer *>& left, std::vector<Renderer *>& right)
    {
        int objs_count = objects.size();
        Bounds centroidBounds = objects[0]->GetBounds();
        for (int i = 0; i < objs_count; ++i)
            centroidBounds.Encapsulate(objects[i]->GetBounds().GetCenter());
        
        int dim = centroidBounds.MaxExtentDim();

        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->GetBounds().GetCenter().x <
                    f2->GetBounds().GetCenter().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->GetBounds().GetCenter().y <
                    f2->GetBounds().GetCenter().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->GetBounds().GetCenter().z <
                    f2->GetBounds().GetCenter().z;
            });
            break;
        }
        auto beginning = objects.begin();
        auto middling = objects.begin() + (objs_count / 2);
        auto ending = objects.end();

        left = std::vector<Renderer *>(beginning, middling);
        right = std::vector<Renderer *>(middling, ending);
    }

    std::shared_ptr<BVHNode> BVHAccel::Build(const Scene& scene, SplitMethod splitMethod)
    {
        if(scene.m_renderers.size() == 0)
            return nullptr;
        time_t start, stop;
        time(&start);
        auto objects = scene.m_renderers;
        auto root = recursiveBuild(objects, splitMethod);

        time(&stop);
        double diff = difftime(stop, start);
        int hrs = (int)diff / 3600;
        int mins = ((int)diff / 60) - (hrs * 60);
        int secs = (int)diff - (hrs * 3600) - (mins * 60);

        std::cout << "BVH Generation took " << hrs << " hours, " << mins << " minutes, " << secs << " seconds\n";
        return root;
    }

    std::shared_ptr<BVHNode> BVHAccel::recursiveBuild(std::vector<Renderer *>& objects, SplitMethod splitMethod)
    {
        std::shared_ptr<BVHNode> node = std::make_shared<BVHNode>();
        Bounds bounds;
        int objs_count = objects.size();
        bounds = objects[0]->GetBounds();
        for (int i = 1; i < objs_count; ++i)
            bounds.Encapsulate(objects[i]->GetBounds());
        if(objs_count == 1)
        {
            node->object = objects[0];
            node->bounds = node->object->GetBounds();
        }else if(objs_count == 2)
        {
            node->left = recursiveBuild(std::vector{objects[0]},splitMethod);
            node->right = recursiveBuild(std::vector{objects[1]},splitMethod);
            node->bounds = Bounds::Union(node->left->bounds, node->right->bounds);
            return node;
        }else
        {
            std::vector<Renderer *> leftPrimitives;
            std::vector<Renderer *> rightPrimitives;
            switch (splitMethod)
            {
            case SplitMethod::SAH:
                throw std::runtime_error("SplitMethod::SAH not implemented");
                break;
            default:
                Partition_NAIVE(objects, leftPrimitives, rightPrimitives);
                break;
            }

            assert(objs_count == (leftPrimitives.size() + rightPrimitives.size()));

            node->left = recursiveBuild(leftPrimitives,splitMethod);
            node->right = recursiveBuild(rightPrimitives,splitMethod);
            node->bounds = Bounds::Union(node->left->bounds, node->right->bounds);
        }
        return node;
    }

}