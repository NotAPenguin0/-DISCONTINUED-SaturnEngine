#ifndef MVG_SKYBOX_HPP_
#define MVG_SKYBOX_HPP_

class Skybox {
public:
    Skybox(const char* fname);

private:
    friend class SkyboxRenderer;

    unsigned int cubemap;
    unsigned int vao;
    unsigned int vbo;
};

#endif