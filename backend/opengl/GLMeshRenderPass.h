//
// Created by alex on 28.02.24.
//

#ifndef ENGINE23_GLMESHRENDERPASS_H
#define ENGINE23_GLMESHRENDERPASS_H


namespace Bcg{
    class SystemRendererOpenGL;

    class GLMeshRenderPass{
    public:
        GLMeshRenderPass() = default;

        ~GLMeshRenderPass() = default;

        void execute();
    private:

        friend SystemRendererOpenGL;

        void prepare();

        void render();

        void post_render();
    };
}

#endif //ENGINE23_GLMESHRENDERPASS_H
