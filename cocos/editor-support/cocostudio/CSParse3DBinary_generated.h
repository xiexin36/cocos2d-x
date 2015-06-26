// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_CSPARSE3DBINARY_FLATBUFFERS_H_
#define FLATBUFFERS_GENERATED_CSPARSE3DBINARY_FLATBUFFERS_H_

#include "flatbuffers/flatbuffers.h"

namespace flatbuffers {
struct CSParseBinary;
struct NodeTree;
struct Options;
struct WidgetOptions;
struct LayoutComponentTable;
struct SingleNodeOptions;
struct SpriteOptions;
struct ParticleSystemOptions;
struct GameMapOptions;
struct ButtonOptions;
struct CheckBoxOptions;
struct ImageViewOptions;
struct TextAtlasOptions;
struct TextBMFontOptions;
struct TextOptions;
struct TextFieldOptions;
struct LoadingBarOptions;
struct SliderOptions;
struct PanelOptions;
struct ScrollViewOptions;
struct PageViewOptions;
struct ListViewOptions;
struct ProjectNodeOptions;
struct ComponentOptions;
struct ComAudioOptions;
struct AnimationInfo;
struct NodeAction;
struct TimeLine;
struct Frame;
struct PointFrame;
struct ScaleFrame;
struct ColorFrame;
struct TextureFrame;
struct EventFrame;
struct IntFrame;
struct BoolFrame;
struct InnerActionFrame;
struct RotationSkew;
struct Position;
struct Scale;
struct AnchorPoint;
struct Color;
struct ColorVector;
struct FlatSize;
struct CapInsets;
struct ResourceData;
}  // namespace flatbuffers

namespace flatbuffers {

struct Node3DOption;
struct Sprite3DOptions;
struct Particle3DOptions;
struct UserCameraOptions;
struct Vector2;
struct Vector3;

MANUALLY_ALIGNED_STRUCT(4) Vector2 {
 private:
  float x_;
  float y_;

 public:
  Vector2(float x, float y)
    : x_(flatbuffers::EndianScalar(x)), y_(flatbuffers::EndianScalar(y)) { }

  float x() const { return flatbuffers::EndianScalar(x_); }
  float y() const { return flatbuffers::EndianScalar(y_); }
};
STRUCT_END(Vector2, 8);

MANUALLY_ALIGNED_STRUCT(4) Vector3 {
 private:
  float x_;
  float y_;
  float z_;

 public:
  Vector3(float x, float y, float z)
    : x_(flatbuffers::EndianScalar(x)), y_(flatbuffers::EndianScalar(y)), z_(flatbuffers::EndianScalar(z)) { }

  float x() const { return flatbuffers::EndianScalar(x_); }
  float y() const { return flatbuffers::EndianScalar(y_); }
  float z() const { return flatbuffers::EndianScalar(z_); }
};
STRUCT_END(Vector3, 12);

struct GameNode3DOption : private flatbuffers::Table {
    const flatbuffers::String *name() const { return GetPointer<const flatbuffers::String *>(4); }
    uint8_t skyBoxEnabled() const { return GetField<uint8_t>(6, 0); }
    const ResourceData *leftFileData() const { return GetPointer<const ResourceData *>(8); }
    const ResourceData *rightFileData() const { return GetPointer<const ResourceData *>(10); }
    const ResourceData *upFileData() const { return GetPointer<const ResourceData *>(12); }
    const ResourceData *downFileData() const { return GetPointer<const ResourceData *>(14); }
    const ResourceData *forwardFileData() const { return GetPointer<const ResourceData *>(16); }
    const ResourceData *backFileData() const { return GetPointer<const ResourceData *>(18); }
    const flatbuffers::String *frameEvent() const { return GetPointer<const flatbuffers::String *>(20); }
    const flatbuffers::String *customProperty() const { return GetPointer<const flatbuffers::String *>(22); }
    bool Verify(flatbuffers::Verifier &verifier) const {
        return VerifyTableStart(verifier) &&
            VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* nodeOptions */) &&
            verifier.Verify(name()) &&
            VerifyField<uint8_t>(verifier, 6 /* skyBoxEnabled */) &&
            VerifyField<flatbuffers::uoffset_t>(verifier, 8 /* leftFileData */) &&
            verifier.VerifyTable(leftFileData()) &&
            VerifyField<flatbuffers::uoffset_t>(verifier, 10 /* rightFileData */) &&
            verifier.VerifyTable(rightFileData()) &&
            VerifyField<flatbuffers::uoffset_t>(verifier, 12 /* upFileData */) &&
            verifier.VerifyTable(upFileData()) &&
            VerifyField<flatbuffers::uoffset_t>(verifier, 14 /* downFileData */) &&
            verifier.VerifyTable(downFileData()) &&
            VerifyField<flatbuffers::uoffset_t>(verifier, 16 /* forwardFileData */) &&
            verifier.VerifyTable(forwardFileData()) &&
            VerifyField<flatbuffers::uoffset_t>(verifier, 18 /* backFileData */) &&
            verifier.VerifyTable(backFileData()) &&
            VerifyField<flatbuffers::uoffset_t>(verifier, 20 /* frameEvent */) &&
            verifier.Verify(frameEvent()) &&
            VerifyField<flatbuffers::uoffset_t>(verifier, 22 /* customProperty */) &&
            verifier.Verify(customProperty()) &&
            verifier.EndTable();
    }
};

struct GameNode3DOptionBuilder {
    flatbuffers::FlatBufferBuilder &fbb_;
    flatbuffers::uoffset_t start_;
    void add_name(flatbuffers::Offset<flatbuffers::String> name) { fbb_.AddOffset(4, name); }
    void add_skyboxEnabled(uint8_t skyBoxEnabled) { fbb_.AddElement<uint8_t>(6, skyBoxEnabled, 0); }
    void add_leftFileData(flatbuffers::Offset<ResourceData> leftFileData) { fbb_.AddOffset(8, leftFileData); }
    void add_rightFileData(flatbuffers::Offset<ResourceData> rightFileData) { fbb_.AddOffset(10, rightFileData); }
    void add_upFileData(flatbuffers::Offset<ResourceData> upFileData) { fbb_.AddOffset(12, upFileData); }
    void add_downFileData(flatbuffers::Offset<ResourceData> downFileData) { fbb_.AddOffset(14, downFileData); }
    void add_forwardFileData(flatbuffers::Offset<ResourceData> forwardFileData) { fbb_.AddOffset(16, forwardFileData); }
    void add_backFileData(flatbuffers::Offset<ResourceData> backFileData) { fbb_.AddOffset(18, backFileData); }
    void add_frameEvent(flatbuffers::Offset<flatbuffers::String> frameEvent) { fbb_.AddOffset(20, frameEvent); }
    void add_customProperty(flatbuffers::Offset<flatbuffers::String> customProperty) { fbb_.AddOffset(22, customProperty); }
    GameNode3DOptionBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
    flatbuffers::Offset<GameNode3DOption> Finish() {
        auto o = flatbuffers::Offset<GameNode3DOption>(fbb_.EndTable(start_, 5));
        return o;
    }
};

inline flatbuffers::Offset<GameNode3DOption> CreateGameNode3DOption(flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<flatbuffers::String> name = 0,
    uint8_t skyBoxEnabled = 0,
    flatbuffers::Offset<ResourceData> leftFileData = 0,
    flatbuffers::Offset<ResourceData> rightFileData = 0,
    flatbuffers::Offset<ResourceData> upFileData = 0,
    flatbuffers::Offset<ResourceData> downFileData = 0,
    flatbuffers::Offset<ResourceData> forwardFileData = 0,
    flatbuffers::Offset<ResourceData> backFileData = 0,
    flatbuffers::Offset<flatbuffers::String> frameEvent = 0,
    flatbuffers::Offset<flatbuffers::String> customProperty = 0) {
    GameNode3DOptionBuilder builder_(_fbb);
    builder_.add_customProperty(customProperty);
    builder_.add_frameEvent(frameEvent);
    builder_.add_backFileData(backFileData);
    builder_.add_forwardFileData(forwardFileData);
    builder_.add_downFileData(downFileData);
    builder_.add_upFileData(upFileData);
    builder_.add_rightFileData(rightFileData);
    builder_.add_leftFileData(leftFileData);
    builder_.add_skyboxEnabled(skyBoxEnabled);
    builder_.add_name(name);
    return builder_.Finish();
}

struct Node3DOption : private flatbuffers::Table {
  const flatbuffers::WidgetOptions *nodeOptions() const { return GetPointer<const flatbuffers::WidgetOptions *>(4); }
  const Vector3 *position3D() const { return GetStruct<const Vector3 *>(6); }
  const Vector3 *rotation3D() const { return GetStruct<const Vector3 *>(8); }
  const Vector3 *scale3D() const { return GetStruct<const Vector3 *>(10); }
  int32_t cameramask() const { return GetField<int32_t>(12, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* nodeOptions */) &&
           verifier.VerifyTable(nodeOptions()) &&
           VerifyField<Vector3>(verifier, 6 /* position3D */) &&
           VerifyField<Vector3>(verifier, 8 /* rotation3D */) &&
           VerifyField<Vector3>(verifier, 10 /* scale3D */) &&
           VerifyField<int32_t>(verifier, 12 /* cameramask */) &&
           verifier.EndTable();
  }
};

struct Node3DOptionBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_nodeOptions(flatbuffers::Offset<flatbuffers::WidgetOptions> nodeOptions) { fbb_.AddOffset(4, nodeOptions); }
  void add_position3D(const Vector3 *position3D) { fbb_.AddStruct(6, position3D); }
  void add_rotation3D(const Vector3 *rotation3D) { fbb_.AddStruct(8, rotation3D); }
  void add_scale3D(const Vector3 *scale3D) { fbb_.AddStruct(10, scale3D); }
  void add_cameramask(int32_t cameramask) { fbb_.AddElement<int32_t>(12, cameramask, 0); }
  Node3DOptionBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  Node3DOptionBuilder &operator=(const Node3DOptionBuilder &);
  flatbuffers::Offset<Node3DOption> Finish() {
    auto o = flatbuffers::Offset<Node3DOption>(fbb_.EndTable(start_, 5));
    return o;
  }
};

inline flatbuffers::Offset<Node3DOption> CreateNode3DOption(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<flatbuffers::WidgetOptions> nodeOptions = 0,
   const Vector3 *position3D = 0,
   const Vector3 *rotation3D = 0,
   const Vector3 *scale3D = 0,
   int32_t cameramask = 0) {
  Node3DOptionBuilder builder_(_fbb);
  builder_.add_cameramask(cameramask);
  builder_.add_scale3D(scale3D);
  builder_.add_rotation3D(rotation3D);
  builder_.add_position3D(position3D);
  builder_.add_nodeOptions(nodeOptions);
  return builder_.Finish();
}

struct Sprite3DOptions : private flatbuffers::Table {
  const Node3DOption *node3DOption() const { return GetPointer<const Node3DOption *>(4); }
  const flatbuffers::ResourceData *fileData() const { return GetPointer<const flatbuffers::ResourceData *>(6); }
  uint8_t runAction() const { return GetField<uint8_t>(8, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* node3DOption */) &&
           verifier.VerifyTable(node3DOption()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 6 /* fileData */) &&
           verifier.VerifyTable(fileData()) &&
           VerifyField<uint8_t>(verifier, 8 /* runAction */) &&
           verifier.EndTable();
  }
};

struct Sprite3DOptionsBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_node3DOption(flatbuffers::Offset<Node3DOption> node3DOption) { fbb_.AddOffset(4, node3DOption); }
  void add_fileData(flatbuffers::Offset<flatbuffers::ResourceData> fileData) { fbb_.AddOffset(6, fileData); }
  void add_runAction(uint8_t runAction) { fbb_.AddElement<uint8_t>(8, runAction, 0); }
  Sprite3DOptionsBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  Sprite3DOptionsBuilder &operator=(const Sprite3DOptionsBuilder &);
  flatbuffers::Offset<Sprite3DOptions> Finish() {
    auto o = flatbuffers::Offset<Sprite3DOptions>(fbb_.EndTable(start_, 3));
    return o;
  }
};

inline flatbuffers::Offset<Sprite3DOptions> CreateSprite3DOptions(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<Node3DOption> node3DOption = 0,
   flatbuffers::Offset<flatbuffers::ResourceData> fileData = 0,
   uint8_t runAction = 0) {
  Sprite3DOptionsBuilder builder_(_fbb);
  builder_.add_fileData(fileData);
  builder_.add_node3DOption(node3DOption);
  builder_.add_runAction(runAction);
  return builder_.Finish();
}

struct Particle3DOptions : private flatbuffers::Table {
  const Node3DOption *node3DOption() const { return GetPointer<const Node3DOption *>(4); }
  const flatbuffers::ResourceData *fileData() const { return GetPointer<const flatbuffers::ResourceData *>(6); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* node3DOption */) &&
           verifier.VerifyTable(node3DOption()) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 6 /* fileData */) &&
           verifier.VerifyTable(fileData()) &&
           verifier.EndTable();
  }
};

struct Particle3DOptionsBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_node3DOption(flatbuffers::Offset<Node3DOption> node3DOption) { fbb_.AddOffset(4, node3DOption); }
  void add_fileData(flatbuffers::Offset<flatbuffers::ResourceData> fileData) { fbb_.AddOffset(6, fileData); }
  Particle3DOptionsBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  Particle3DOptionsBuilder &operator=(const Particle3DOptionsBuilder &);
  flatbuffers::Offset<Particle3DOptions> Finish() {
    auto o = flatbuffers::Offset<Particle3DOptions>(fbb_.EndTable(start_, 2));
    return o;
  }
};

inline flatbuffers::Offset<Particle3DOptions> CreateParticle3DOptions(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<Node3DOption> node3DOption = 0,
   flatbuffers::Offset<flatbuffers::ResourceData> fileData = 0) {
  Particle3DOptionsBuilder builder_(_fbb);
  builder_.add_fileData(fileData);
  builder_.add_node3DOption(node3DOption);
  return builder_.Finish();
}

struct UserCameraOptions : private flatbuffers::Table {
  const Node3DOption *node3DOption() const { return GetPointer<const Node3DOption *>(4); }
  float fov() const { return GetField<float>(6, 60); }
  float nearClip() const { return GetField<float>(8, 1); }
  float farClip() const { return GetField<float>(10, 1000); }
  int32_t cameraFlag() const { return GetField<int32_t>(12, 0); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<flatbuffers::uoffset_t>(verifier, 4 /* node3DOption */) &&
           verifier.VerifyTable(node3DOption()) &&
           VerifyField<float>(verifier, 6 /* fov */) &&
           VerifyField<float>(verifier, 8 /* nearClip */) &&
           VerifyField<float>(verifier, 10 /* farClip */) &&
           VerifyField<int32_t>(verifier, 12 /* cameraFlag */) &&
           verifier.EndTable();
  }
};

struct UserCameraOptionsBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_node3DOption(flatbuffers::Offset<Node3DOption> node3DOption) { fbb_.AddOffset(4, node3DOption); }
  void add_fov(float fov) { fbb_.AddElement<float>(6, fov, 60); }
  void add_nearClip(float nearClip) { fbb_.AddElement<float>(8, nearClip, 1); }
  void add_farClip(float farClip) { fbb_.AddElement<float>(10, farClip, 1000); }
  void add_cameraFlag(int32_t cameraFlag) { fbb_.AddElement<int32_t>(12, cameraFlag, 0); }
  UserCameraOptionsBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  UserCameraOptionsBuilder &operator=(const UserCameraOptionsBuilder &);
  flatbuffers::Offset<UserCameraOptions> Finish() {
    auto o = flatbuffers::Offset<UserCameraOptions>(fbb_.EndTable(start_, 5));
    return o;
  }
};

inline flatbuffers::Offset<UserCameraOptions> CreateUserCameraOptions(flatbuffers::FlatBufferBuilder &_fbb,
   flatbuffers::Offset<Node3DOption> node3DOption = 0,
   float fov = 60,
   float nearClip = 1,
   float farClip = 1000,
   int32_t cameraFlag = 0) {
  UserCameraOptionsBuilder builder_(_fbb);
  builder_.add_cameraFlag(cameraFlag);
  builder_.add_farClip(farClip);
  builder_.add_nearClip(nearClip);
  builder_.add_fov(fov);
  builder_.add_node3DOption(node3DOption);
  return builder_.Finish();
}

inline const Node3DOption *GetNode3DOption(const void *buf) { return flatbuffers::GetRoot<Node3DOption>(buf); }

inline bool VerifyNode3DOptionBuffer(flatbuffers::Verifier &verifier) { return verifier.VerifyBuffer<Node3DOption>(); }

inline void FinishNode3DOptionBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<Node3DOption> root) { fbb.Finish(root); }

}  // namespace flatbuffers

#endif  // FLATBUFFERS_GENERATED_CSPARSE3DBINARY_FLATBUFFERS_H_
