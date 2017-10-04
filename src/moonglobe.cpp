#include "moonglobe.h"

using namespace Magnum;

MoonGlobe::MoonGlobe(const Arguments &arguments)
    : Platform::Application{arguments, Configuration{}.setTitle("sphere")}
    , _shader{Shaders::Phong::Flag::DiffuseTexture}
{
    //enable depthtest and faceculling
    Renderer::enable(Renderer::Feature::DepthTest);
    Renderer::enable(Renderer::Feature::FaceCulling);

    // create sphere mesh
    const Trade::MeshData3D sphere = Primitives::UVSphere::solid(128, 128, Primitives::UVSphere::TextureCoords::Generate);
    _vertexBuffer.setData(MeshTools::interleave(sphere.positions(0),
                                                sphere.normals(0),
                                                sphere.textureCoords2D(0)),
                          BufferUsage::StaticDraw);

    Containers::Array<char> indexData;
    Mesh::IndexType indexType;
    UnsignedInt indexStart, indexEnd;
    // feel index buffer
    std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(sphere.indices());
    _indexBuffer.setData(indexData, BufferUsage::StaticDraw);

    // configure mesh params
    _mesh.setPrimitive(sphere.primitive())
        .setCount(sphere.indices().size())
        .addVertexBuffer(_vertexBuffer, 0, Shaders::Phong::Position{},
                                           Shaders::Phong::Normal{},
                                           Shaders::Phong::TextureCoordinates{})
        .setIndexBuffer(_indexBuffer, 0, indexType, indexStart, indexEnd);

    // set transform matrix
    _transformation = Matrix4::rotationX(30.0_degf) *
        Matrix4::rotationY(40.0_degf);
    _color = Color3::fromHsv(35.0_degf, 1.0f, 1.0f);
 
    // set perspective projection
    _projection = Matrix4::perspectiveProjection(
        35.0_degf,
        Vector2{defaultFramebuffer.viewport().size()}.aspectRatio(), 0.01f, 100.0f)
        * Matrix4::translation(Vector3::zAxis(-10.0f));

    // load texture
    PluginManager::Manager<Trade::AbstractImporter> manager{MAGNUM_PLUGINS_IMPORTER_DIR};
    std::unique_ptr<Trade::AbstractImporter> importer = manager.loadAndInstantiate("JpegImporter");
    if (!importer) {
        Error() << "Cannot load importer plugin from" << manager.pluginDirectory();
        std::exit(1);
    }

    const Utility::Resource rs{"sphere-data"};
    if(!importer->openData(rs.getRaw("moon.jpg")))
        std::exit(2);

    std::optional<Trade::ImageData2D> image = importer->image2D(0);
    CORRADE_INTERNAL_ASSERT(image);

/*    if (image -> format() == PixelFormat::Red) {
        Error() << "RED";
    } else if (image -> format() == PixelFormat::RGB) {
        Error() << "RGB";
    }*/
    // set texture params
    texture_.setWrapping(Sampler::Wrapping::MirroredRepeat)
        .setMagnificationFilter(Sampler::Filter::Linear)
        .setMinificationFilter(Sampler::Filter::Linear)
        .setStorage(1, TextureFormat::R8, image->size())
        .setSubImage(0, {}, *image);
}

void MoonGlobe::drawEvent()
{
    defaultFramebuffer.clear(FramebufferClear::Color | FramebufferClear::Depth);

    // set shaders uniforms
    _shader.setLightPosition({0.0f,1.0f, 5.0f})
        .setDiffuseTexture(texture_)
        .setLightColor(Color3{1.0f, 1.0f, 1.0f})
        .setTransformationMatrix(_transformation)
        .setNormalMatrix(_transformation.rotationScaling())
        .setProjectionMatrix(_projection);
    _mesh.draw(_shader);

    swapBuffers();
}

void MoonGlobe::mousePressEvent(MouseEvent &event)
{
    if (event.button() != MouseEvent::Button::Left) return;

    _previousMousePosition = event.position();
    event.setAccepted();
}

void MoonGlobe::mouseReleaseEvent(MouseEvent &event)
{
    _color = Color3::fromHsv(_color.hue() + 50.0_degf, 1.0f, 1.0f);

    event.setAccepted();
    redraw();
}

void MoonGlobe::mouseMoveEvent(MouseMoveEvent &event)
{
    if (!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    const Vector2 delta = 3.0f *
        Vector2{event.position() - _previousMousePosition} /
        Vector2{defaultFramebuffer.viewport().size()};

    _transformation =
        Matrix4::rotationX(Rad{delta.y()}) *
            _transformation *
            Matrix4::rotationY(Rad{delta.x()});

    _previousMousePosition = event.position();
    event.setAccepted();
    redraw();
}

