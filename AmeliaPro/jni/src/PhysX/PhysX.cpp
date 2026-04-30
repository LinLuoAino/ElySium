#include "PhysX.h"
#include "辅助类.h"
#include "VectorTools.h"
#include <algorithm>
#include <memory>
#include <iostream>
#include <thread>
#include <numeric>
#include <sstream>
#include "foundation/PxTransform.h"
#include "foundation/PxVec3.h"
#include "foundation/PxVec4.h"

extern 绘制 绘制;

std::mutex Physics::sceneMutex;

namespace {
    constexpr uint64_t PhysXPtr = 0x157B9FE0;
    constexpr uint64_t RigidActors = 0x2568;
    constexpr uint64_t SQManager = 0x2430;
    constexpr uint64_t VehicleWheelOffset = 0x3A0;
    
    template <typename T>
    struct TArray {
        uintptr_t base;
        int32_t count;
        int32_t max;
        
        [[nodiscard]] std::vector<T> ToVec() const {
            if (!IsValid()) {
                return {};
            }
            std::vector<T> vec{};
            vec.resize(static_cast<size_t>(count));
            return vec;
        }
        
        T operator[](size_t u) const {
            return 绘制.读写.read<T>(base + u * sizeof(T));
        }
        
        [[nodiscard]] bool IsValid() const {
            return base && count > 0 && count <= max && max > 0;
        }
    };
}

namespace PhysX {
    enum class PxConcreteType : uint16_t {
        eUNDEFINED,
        eHEIGHTFIELD,
        eCONVEX_MESH,
        eTRIANGLE_MESH_BVH33,
        eTRIANGLE_MESH_BVH34,
        eCLOTH_FABRIC,
        eRIGID_DYNAMIC,
        eRIGID_STATIC,
        eSHAPE,
        eMATERIAL,
        eCONSTRAINT,
        eCLOTH,
        ePARTICLE_SYSTEM,
        ePARTICLE_FLUID,
        eAGGREGATE,
        eARTICULATION,
        eARTICULATION_LINK,
        eARTICULATION_JOINT,
        ePRUNING_STRUCTURE,
        ePHYSX_CORE_COUNT,
        eFIRST_PHYSX_EXTENSION = 256,
        eFIRST_VEHICLE_EXTENSION = 512,
        eFIRST_USER_EXTENSION = 1024
    };
    
    enum class PxBaseFlag : uint16_t {
        eOWNS_MEMORY = (1 << 0),
        eIS_RELEASABLE = (1 << 1)
    };
    
    enum class PxRigidBodyFlag : uint8_t {
        eKINEMATIC = (1 << 0),
        eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES = (1 << 1),
        eENABLE_CCD = (1 << 2),
        eENABLE_CCD_FRICTION = (1 << 3),
        eENABLE_POSE_INTEGRATION_PREVIEW = (1 << 4),
        eENABLE_SPECULATIVE_CCD = (1 << 5),
        eENABLE_CCD_MAX_CONTACT_IMPULSE = (1 << 6)
    };
    
    struct PxMatrix3x3T {
        physx::PxVec3 column0{};
        physx::PxVec3 column1{};
        physx::PxVec3 column2{};
        
        PxMatrix3x3T() = default;
        
        explicit PxMatrix3x3T(const physx::PxVec4& q) {
            const float x = q.x;
            const float y = q.y;
            const float z = q.z;
            const float w = q.w;
            
            const float x2 = x + x;
            const float y2 = y + y;
            const float z2 = z + z;
            
            const float xx = x2 * x;
            const float yy = y2 * y;
            const float zz = z2 * z;
            
            const float xy = x2 * y;
            const float xz = x2 * z;
            const float xw = x2 * w;
            
            const float yz = y2 * z;
            const float yw = y2 * w;
            const float zw = z2 * w;
            
            column0 = physx::PxVec3(1.0f - yy - zz, xy + zw, xz - yw);
            column1 = physx::PxVec3(xy - zw, 1.0f - xx - zz, yz + xw);
            column2 = physx::PxVec3(xz + yw, yz - xw, 1.0f - xx - yy);
        }
        
        PxMatrix3x3T(const physx::PxVec3& col0, const physx::PxVec3& col1, const physx::PxVec3& col2)
            : column0(col0), column1(col1), column2(col2) {}
        
        [[nodiscard]] PxMatrix3x3T getTranspose() const {
            const physx::PxVec3 v0(column0.x, column1.x, column2.x);
            const physx::PxVec3 v1(column0.y, column1.y, column2.y);
            const physx::PxVec3 v2(column0.z, column1.z, column2.z);
            
            return {v0, v1, v2};
        }
        
        [[nodiscard]] physx::PxVec3 transform(const physx::PxVec3& other) const {
            return column0 * other.x + column1 * other.y + column2 * other.z;
        }
        
        PxMatrix3x3T operator*(const PxMatrix3x3T& other) const {
            return {transform(other.column0), transform(other.column1), transform(other.column2)};
        }
        
        physx::PxVec3 operator*(const physx::PxVec3& vec) const {
            return transform(vec);
        }
    };
    
    struct PxsRigidCoreT {
        alignas(16) physx::PxTransform mBodyToWorld;
        PxRigidBodyFlag Flags{};
        uint8_t m_idt_body_to_actor{};
        uint16_t m_solver_iteration_counts{};
    };
    
    struct BodyCoreT {
        char mPad[0x10]{};
        alignas(16) PxsRigidCoreT mCore{};
        alignas(16) physx::PxTransform mBodyToActor;
    };
    
    struct BodyT {
        char mPad[0x60]{};
        uint64_t mScene{};
        uint64_t mControlState{};
        uint64_t mStreamPtr{};
        BodyCoreT mRigid{};
    };
    
    struct VehicleWheelT {
        char mPad[VehicleWheelOffset]{};
        uint64_t mShape{};
        uint64_t mActor{};
        float mRadius{};
        float mWidth{};
        float mSuspensionTravel{};
    };
    
    struct PxActorT {
        char mPad[0x8]{};
        PxConcreteType mType{};
        PxBaseFlag mBaseFlags{};
    };
    
    struct PxGeometryT {
        PxGeometryType mType{};
    };
    
    struct PxBoxGeometry : PxGeometryT {
        physx::PxVec3 mHalfExtents{};
        
        bool valid() const {
            if (mType != PxGeometryType::eBOX) return false;
            if (mHalfExtents.x <= 0.0f || mHalfExtents.y <= 0.0f || mHalfExtents.z <= 0.0f) return false;
            return true;
        }
    };
    
    struct PxSphereGeometryT : PxGeometryT {
        float mRadius{};
        
        bool valid() const {
            if (mType != PxGeometryType::eSPHERE) return false;
            if (mRadius <= 0.f) return false;
            return true;
        }
    };
    
    struct PxCapsuleGeometryT : PxGeometryT {
        float mRadius{};
        float mHalfHeight{};
        
        bool valid() const {
            if (mType != PxGeometryType::eCAPSULE) return false;
            if (mRadius <= 0.f || mHalfHeight <= 0.f) return false;
            return true;
        }
    };
    
    struct PxMeshScale {
        physx::PxVec3 scale;
        physx::PxQuat rotation;
        
        physx::PxVec3 transform(const physx::PxVec3& v) const {
            return rotation.rotateInv(scale.multiply(rotation.rotate(v)));
        }
    };
    
    struct CenterExtentsT {
        physx::PxVec3 mCenter{};
        physx::PxVec3 mExtents{};
    };
    
    struct PxPlaneT {
        physx::PxVec3 n;
        float d{};
        
        bool operator==(const PxPlaneT& p) const {
            return n == p.n && d == p.d;
        }
    };
    
    struct HullPolygonDataT {
        PxPlaneT mPlane;
        uint16_t mVRef8{};
        uint8_t mNbVerts{};
        uint8_t mMinIndex{};
    };
    
    template <typename storageType, storageType bitMask>
    class PxBitAndDataT {
    public:
        PxBitAndDataT() : mData(0) {}
        PxBitAndDataT(storageType data, bool bit = false) {
            mData = bit ? storageType(data | bitMask) : data;
        }
        
        operator storageType() const {
            return storageType(mData & ~bitMask);
        }
        
        void setBit() { mData |= bitMask; }
        void clearBit() { mData &= ~bitMask; }
        storageType isBitSet() const { return storageType(mData & bitMask); }
        
    protected:
        storageType mData;
    };
    
    typedef PxBitAndDataT<unsigned char, 0x80> PxBitAndByte;
    typedef PxBitAndDataT<unsigned short, 0x8000> PxBitAndWord;
    typedef PxBitAndDataT<unsigned int, 0x80000000> PxBitAndDword;
    
    template<uint8_t TNumBytes>
    struct PxPadding {
        uint8_t mPadding[TNumBytes]{};
        PxPadding() {
            for (uint8_t idx = 0; idx < TNumBytes; ++idx)
                mPadding[idx] = 0;
        }
    };
    
    struct ConvexHullDataT {
        CenterExtentsT mAABB{};
        physx::PxVec3 m_center_of_mass{};
        PxBitAndWord mNbEdges{};
        uint8_t HullVerticesNb{};
        uint8_t PolygonsNb{};
        HullPolygonDataT* mPolygons{};
        
        const physx::PxVec3* getHullVertices() const {
            const char* tmp = reinterpret_cast<const char*>(mPolygons);
            tmp += sizeof(HullPolygonDataT) * PolygonsNb;
            return reinterpret_cast<const physx::PxVec3*>(tmp);
        }
        
        const uint8_t* getVertexData8() const {
            const char* tmp = reinterpret_cast<const char*>(mPolygons);
            tmp += sizeof(HullPolygonDataT) * PolygonsNb;
            tmp += sizeof(physx::PxVec3) * HullVerticesNb;
            tmp += sizeof(uint8_t) * mNbEdges * 2;
            tmp += sizeof(uint8_t) * HullVerticesNb * 3;
            if (mNbEdges.isBitSet())
                tmp += sizeof(uint16_t) * mNbEdges * 2;
            return reinterpret_cast<const uint8_t*>(tmp);
        }
    };
    
    struct ConvexMeshT {
        char mPad[0x8]{};
        PxConcreteType mType{};
        PxBaseFlag mBaseFlags{};
        uint64_t mRefCountableVfptr{};
        int32_t mRefCount{};
        ConvexHullDataT HullData{};
        uint32_t mNb{};
    };
    
    struct PxConvexMeshGeometryT : PxGeometryT {
        PxMeshScale Scale{};
        ConvexMeshT* ConvexMesh{};
        float mMaxMargin{};
        uint8_t mMeshFlags{};
        PxPadding<3> paddingFromFlags;
    };
    
    struct TriangleMeshT {
        char mPad[0x8]{};
        PxConcreteType mType{};
        PxBaseFlag mBaseFlags{};
        uint64_t mRefCountableVfptr{};
        int32_t mRefCount{};
        uint32_t mNbVertices{};
        uint32_t mNbTriangles{};
        physx::PxVec3* Vertices{};
        void* mTriangles{};
        CenterExtentsT mAABB{};
        uint8_t* mExtraTrigData{};
        float mGeomEpsilon{};
        uint8_t Flags{};
    };
    
    struct PxTriangleMeshGeometryT : PxGeometryT {
        PxMeshScale Scale{};
        uint8_t mMeshFlags{};
        PxPadding<3> paddingFromFlags;
        TriangleMeshT* mTriangleMesh{};
    };
    
    struct PxHeightFieldSampleT {
        int16_t mHeight{};
        PxBitAndByte mMaterialIndex0{};
        PxBitAndByte mMaterialIndex1{};
    };
    
    struct HeightFieldDataT {
        CenterExtentsT mAABB{};
        uint32_t Rows{};
        uint32_t Columns{};
        float mRowLimit{};
        float mColumnLimit{};
        float mNbColumns{};
        PxHeightFieldSampleT* mSamples{};
        float mThickness{};
        float mConvexEdgeThreshold{};
        uint16_t mFlags{};
        uint8_t mFormat{};
    };
    
    struct HeightFieldT {
        char mPad[0x8]{};
        PxConcreteType mType{};
        PxBaseFlag mBaseFlags{};
        uint64_t mRefCountableVfptr{};
        int32_t mRefCount{};
        HeightFieldDataT mData{};
        uint32_t mSampleStride{};
        uint32_t mNbSamples{};
        float mMinHeight{};
        float mMaxHeight{};
        int32_t mModifyCount{};
        void* mMeshFactory{};
    };
    
    struct PxHeightFieldGeometryT : PxGeometryT {
        HeightFieldT* mHeightField{};
        float HeightScale{};
        float RowScale{};
        float ColumnScale{};
        int8_t Flags{};
        PxPadding<3> paddingFromFlags;
    };
    
    struct GeometryUnionT {
        union {
            void* alignment;
            uint8_t box[16];
            uint8_t sphere[8];
            uint8_t capsule[12];
            uint8_t plane[4];
            uint8_t convex[64];
            uint8_t mesh[80];
            uint8_t heightfield[56];
            uint8_t invalid[4];
        } mGeometry;
        
        PxGeometryType getType() const {
            return reinterpret_cast<const PxGeometryT&>(mGeometry).mType;
        }
    };
    
    struct PxShapeCoreT {
        alignas(16) physx::PxTransform transform;
        float contactOffset;
        uint8_t mShapeFlags;
        uint8_t mOwnsMaterialIdxMemory;
        uint16_t materialIndex;
        GeometryUnionT geometry;
    };
    
    struct ShapeCoreT {
        FilterDataT QueryFilterData{};
        FilterDataT SimulationFilterData{};
        alignas(16) PxShapeCoreT mCore{};
        float mRestOffset{};
        
        PxGeometryType getGeometryType() const {
            return mCore.geometry.getType();
        }
        
        const physx::PxTransform getShape2Actor() const {
            return mCore.transform;
        }
    };
    
    struct ShapeT {
        char mPad[0x30]{};
        uint64_t mScene{};
        uint32_t mControlState{};
        uint64_t mStreamPtr{};
        ShapeCoreT ShapeCore{};
        
        const PxGeometryT& GetGeometry() const {
            return reinterpret_cast<const PxGeometryT&>(ShapeCore.mCore.geometry.mGeometry);
        }
    };
    
    struct PxBounds3 {
        physx::PxVec3 min, max;
    };
    
    struct PruningPoolT {
        uint32_t mNbObjects;
        uint32_t mMaxNbObjects;
        PxBounds3* mWorldBoxes;
        PrunerPayload* mObjects;
    };
    
    struct PrunerExtT {
        uint64_t mPruner{};
        char mPad[0x24]{};
        uint32_t mTimestamp{};
    };
    
    struct NpSceneT {
        char mPad[SQManager]{};
        PrunerExtT exts[2];
    };
    
    struct ShapeDataT {
        ShapeT Shape{};
        BodyT Actor{};
        PxGeometryType mType{};
        uint8_t m_shape_flags{};
        PxMeshScale Scale{};
        PrunerPayload UniqueKey{};
        ConvexMeshT ConvexMesh{};
        std::vector<HullPolygonDataT> polygons{};
        std::vector<uint8_t> ConvexIndices{};
        std::vector<uint32_t> Indices{};
        std::vector<uint16_t> SmallIndices{};
        TriangleMeshT TriangleMesh{};
        std::vector<physx::PxVec3> Vertices{};
        std::vector<PxHeightFieldSampleT> mSamples{};
        HeightFieldT mHeightField{};
        bool bIsVehicleWheel{false};
        float WheelRadius{0.0f};
        float WheelWidth{0.0f};
    };
}

static uint64_t g_libUE4 = 0;

template<typename T>
void AddReadVecScatter(uint64_t address, size_t size, std::vector<T>* vec) {
    if (!size) return;
    vec->resize(size);
    绘制.读写.read(address, vec->data(), sizeof(T) * size);
}

static std::vector<PrunerPayload> CollectAllVehicleWheelShapes() {
    std::vector<PrunerPayload> result;
    uint64_t PhysxInstancePtr = 绘制.读写.read<uint64_t>(g_libUE4 + PhysXPtr);
    TArray<uint64_t> physx_scenes = 绘制.读写.read<TArray<uint64_t>>(PhysxInstancePtr + 0x8);
    
    for (int i = 0; i < physx_scenes.count; i++) {
        uint64_t scenes_ptr = 绘制.读写.read<uint64_t>(physx_scenes.base + i * sizeof(uint64_t));
        TArray<uint64_t> actors = 绘制.读写.read<TArray<uint64_t>>(scenes_ptr + RigidActors);
        
        for (int j = 0; j < actors.count; j++) {
            uint64_t actor_ptr = 绘制.读写.read<uint64_t>(actors.base + j * sizeof(uint64_t));
            uint16_t Actor_Type = 绘制.读写.read<uint16_t>(actor_ptr + 0x8);
            if (Actor_Type != 6) continue;
            
            uint64_t shape_manager = actor_ptr + 0x28;
            uint64_t shape_ptr = 绘制.读写.read<uint64_t>(shape_manager + 0x0);
            
            uint64_t vehicle_ptr = actor_ptr - 0x30;
            uint16_t vehicle_check = 绘制.读写.read<uint16_t>(vehicle_ptr);
            
            if (vehicle_check == 0x14 || vehicle_check == 0x15) {
                PrunerPayload payload;
                payload.Shape = shape_ptr;
                payload.Actor = actor_ptr;
                payload.bIsVehicle = true;
                result.push_back(payload);
            }
        }
    }
    
    return result;
}

static std::vector<PrunerPayload> CollectAllDynamicRigidActorShapes() {
    std::vector<PrunerPayload> result;
    uint64_t PhysxInstancePtr = 绘制.读写.read<uint64_t>(g_libUE4 + PhysXPtr);
    if (!PhysxInstancePtr) return result;

    TArray<uint64_t> physx_scenes = 绘制.读写.read<TArray<uint64_t>>(PhysxInstancePtr + 0x8);
    
    for (int i = 0; i < physx_scenes.count; i++) {
        uint64_t scenes_ptr = 绘制.读写.read<uint64_t>(physx_scenes.base + i * sizeof(uint64_t));
        if (!scenes_ptr) continue;

        TArray<uint64_t> actors = 绘制.读写.read<TArray<uint64_t>>(scenes_ptr + RigidActors);
        
        for (int j = 0; j < actors.count; j++) {
            uint64_t actor_ptr = 绘制.读写.read<uint64_t>(actors.base + j * sizeof(uint64_t));
            if (!actor_ptr) continue;

            uint16_t Actor_Type = 绘制.读写.read<uint16_t>(actor_ptr + 0x8);
            
            if (Actor_Type == 6) {
                uint64_t vehicle_ptr = actor_ptr - 0x30;
                uint16_t vehicle_check = 绘制.读写.read<uint16_t>(vehicle_ptr);
                
                bool isVehicle = (vehicle_check == 0x14 || vehicle_check == 0x15);

                uint64_t shape_manager = actor_ptr + 0x28; 
                uint64_t shape_ptr = 绘制.读写.read<uint64_t>(shape_manager);
                
                if (shape_ptr) {
                    PhysX::ShapeT temp_shape;
                    if (绘制.读写.read(shape_ptr, &temp_shape, sizeof(PhysX::ShapeT))) {
                        PxGeometryType type = temp_shape.ShapeCore.getGeometryType();

                        if (type == PxGeometryType::eBOX || 
                            type == PxGeometryType::eCONVEXMESH || 
                            type == PxGeometryType::eTRIANGLEMESH) {
                            
                            PrunerPayload payload;
                            payload.Shape = shape_ptr;
                            payload.Actor = actor_ptr;
                            payload.bIsVehicle = isVehicle;
                            result.push_back(payload);
                        }
                    }
                }
            }
        }
    }
    return result;
}

static std::vector<PrunerPayload> CollectAllHeightRigidActorShapes() {
    std::vector<PrunerPayload> result;
    uint64_t PhysxInstancePtr = 绘制.读写.read<uint64_t>(g_libUE4 + PhysXPtr);
    TArray<uint64_t> physx_scenes = 绘制.读写.read<TArray<uint64_t>>(PhysxInstancePtr + 0x8);
    
    for (int i = 0; i < physx_scenes.count; i++) {
        uint64_t scenes_ptr = 绘制.读写.read<uint64_t>(physx_scenes.base + i * sizeof(uint64_t));
        TArray<uint64_t> actors = 绘制.读写.read<TArray<uint64_t>>(scenes_ptr + RigidActors);
        
        for (int j = 0; j < actors.count; j++) {
            uint64_t actor_ptr = 绘制.读写.read<uint64_t>(actors.base + j * sizeof(uint64_t));
            uint16_t Actor_Type = 绘制.读写.read<uint16_t>(actor_ptr + 0x8);
            if (Actor_Type != 7) continue;
            
            uint64_t shape_manager = actor_ptr + 0x28;
            uint64_t shape_ptr = 绘制.读写.read<uint64_t>(shape_manager + 0x0);
            PrunerPayload payload;
            payload.Shape = shape_ptr;
            payload.Actor = actor_ptr;
            payload.bIsVehicle = false;
            result.push_back(payload);
        }
    }
    
    return result;
}

static std::vector<PrunerPayload> CollectAllStaticRigidActorShapes() {
    std::vector<PrunerPayload> result;
    uint64_t PhysxInstancePtr = 绘制.读写.read<uint64_t>(g_libUE4 + PhysXPtr);
    TArray<uint64_t> physx_scenes = 绘制.读写.read<TArray<uint64_t>>(PhysxInstancePtr + 0x8);
    
    for (int i = 0; i < physx_scenes.count; i++) {
        uint64_t scenes_ptr = 绘制.读写.read<uint64_t>(physx_scenes.base + i * sizeof(uint64_t));
        TArray<uint64_t> actors = 绘制.读写.read<TArray<uint64_t>>(scenes_ptr + RigidActors);
        
        for (int j = 0; j < actors.count; j++) {
            uint64_t actor_ptr = 绘制.读写.read<uint64_t>(actors.base + j * sizeof(uint64_t));
            uint16_t Actor_Type = 绘制.读写.read<uint16_t>(actor_ptr + 0x8);
            if (Actor_Type != 7) continue;
            
            uint64_t shape_manager = actor_ptr + 0x28;
            uint64_t shape_ptr = 绘制.读写.read<uint64_t>(shape_manager + 0x0);
            PrunerPayload payload;
            payload.Shape = shape_ptr;
            payload.Actor = actor_ptr;
            payload.bIsVehicle = false;
            result.push_back(payload);
        }
    }
    
    return result;
}

void initue4(uint64_t libue4) {
    g_libUE4 = libue4;
}

template <class FilterFunc>
std::vector<TriangleMeshData> GetMeshData(
    std::vector<PrunerPayload>& objects,
    FilterFunc filter,
    bool isDynamic = false,
    bool autoTransform = true
) {
    std::vector<std::unique_ptr<PhysX::ShapeDataT>> ShapeDataPtrs;
    std::vector<TriangleMeshData> TriangleMeshDatas;
    ShapeDataPtrs.reserve(objects.size());

    // 收集并过滤数据
    for (const auto& obj : objects) {
        auto data = std::make_unique<PhysX::ShapeDataT>();
        data->UniqueKey = obj;
        data->bIsVehicleWheel = obj.bIsVehicle;

        if (!绘制.读写.read(obj.Shape, &data->Shape, sizeof(PhysX::ShapeT))) continue;
        if (!绘制.读写.read(obj.Actor, &data->Actor, sizeof(PhysX::BodyT))) continue;

        if (obj.bIsVehicle) {
            uint64_t wheel_ptr = obj.Actor - 0x30;
            PhysX::VehicleWheelT wheel;
            if (绘制.读写.read(wheel_ptr, &wheel, sizeof(PhysX::VehicleWheelT))) {
                data->WheelRadius = wheel.mRadius;
                data->WheelWidth = wheel.mWidth;
            }
        }

        data->m_shape_flags = data->Shape.ShapeCore.mCore.mShapeFlags;
        data->mType = data->Shape.ShapeCore.getGeometryType();
        
        if (filter(*data)) {
            ShapeDataPtrs.push_back(std::move(data));
        }
    }

    ShapeDataPtrs.erase(
        std::remove_if(ShapeDataPtrs.begin(), ShapeDataPtrs.end(),
            [](const std::unique_ptr<PhysX::ShapeDataT>& ptr) -> bool {
                const auto& shapeData = *ptr;
                const uint32_t eTRIGGER_SHAPE = 0x00000008;
                
                if (!(shapeData.m_shape_flags & eTRIGGER_SHAPE)) {
                    return false;
                }
                
                float sizeEstimate = 0.0f;
                
                try {
                    if (shapeData.mType == PxGeometryType::eBOX) {
                        auto& boxGeometry = static_cast<const PhysX::PxBoxGeometry&>(shapeData.Shape.GetGeometry());
                        sizeEstimate = std::max({boxGeometry.mHalfExtents.x, boxGeometry.mHalfExtents.y, boxGeometry.mHalfExtents.z}) * 2.0f;
                    }
                    else {
                        return true;
                    }
                }
                catch (...) {
                    return true;
                }
                
                if (sizeEstimate < 150.0f || sizeEstimate > 750.0f) {
                    return true;
                }
                
                return false;
            }
        ),
        ShapeDataPtrs.end()
    );
    
    for (auto& ptr : ShapeDataPtrs) {
        auto& ShapeData = *ptr;
        if (ShapeData.mType == PxGeometryType::eHEIGHTFIELD) {
            PhysX::PxHeightFieldGeometryT field_geometry = (PhysX::PxHeightFieldGeometryT&)ShapeData.Shape.GetGeometry();
            绘制.读写.read((uint64_t)field_geometry.mHeightField, &ShapeData.mHeightField, sizeof(PhysX::HeightFieldT));
        } else if (ShapeData.mType == PxGeometryType::eCONVEXMESH) {
            PhysX::PxConvexMeshGeometryT convex_geometry = (PhysX::PxConvexMeshGeometryT&)ShapeData.Shape.GetGeometry();
            ShapeData.Scale = convex_geometry.Scale;
            绘制.读写.read((uint64_t)convex_geometry.ConvexMesh, &ShapeData.ConvexMesh, sizeof(PhysX::ConvexMeshT));
        }
    }
    
    // 读取凸网格的多边形数据
    for (auto& ptr : ShapeDataPtrs) {
        auto& shapeData = *ptr;
        if (shapeData.mType == PxGeometryType::eCONVEXMESH) {
            auto nbPolygons = shapeData.ConvexMesh.HullData.PolygonsNb;
            auto mPolygons = (uint64_t)shapeData.ConvexMesh.HullData.mPolygons;
            auto size = sizeof(PhysX::HullPolygonDataT);
            shapeData.polygons.resize(nbPolygons);
            for (uint32_t i = 0; i < nbPolygons; i++) {
                shapeData.polygons[i] = 绘制.读写.read<PhysX::HullPolygonDataT>(mPolygons + (i * size));
            }
        }
    }
    
    // 读取顶点和索引数据
    for (auto& ptr : ShapeDataPtrs) {
        auto& ShapeData = *ptr;
        if (ShapeData.mType == PxGeometryType::eCONVEXMESH) {
            uint32_t indices_number = std::accumulate(ShapeData.polygons.begin(), ShapeData.polygons.end(), 0u,
                [](uint32_t sum, const PhysX::HullPolygonDataT& polygon) {
                    return sum + polygon.mNbVerts;
                });
            ShapeData.ConvexIndices.reserve(indices_number);
            AddReadVecScatter<uint8_t>((uint64_t)ShapeData.ConvexMesh.HullData.getVertexData8(), indices_number, &ShapeData.ConvexIndices);
        } else if (ShapeData.mType == PxGeometryType::eTRIANGLEMESH) {
            const PhysX::PxTriangleMeshGeometryT& triangle_geometry = (PhysX::PxTriangleMeshGeometryT&)ShapeData.Shape.GetGeometry();
            ShapeData.Scale = triangle_geometry.Scale;
            ShapeData.TriangleMesh = 绘制.读写.read<PhysX::TriangleMeshT>((uintptr_t)triangle_geometry.mTriangleMesh);
        }
    }
    
    // 读取具体的顶点和索引数据
    for (auto& ptr : ShapeDataPtrs) {
        auto& ShapeData = *ptr;
        if (ShapeData.mType == PxGeometryType::eTRIANGLEMESH) {
            bool has16BitIndices = (ShapeData.TriangleMesh.Flags & 2U) ? true : false;
            AddReadVecScatter<physx::PxVec3>((uint64_t)ShapeData.TriangleMesh.Vertices, ShapeData.TriangleMesh.mNbVertices, &ShapeData.Vertices);
            if (has16BitIndices) {
                AddReadVecScatter<uint16_t>((uint64_t)ShapeData.TriangleMesh.mTriangles, ShapeData.TriangleMesh.mNbTriangles * 3, &ShapeData.SmallIndices);
            } else {
                AddReadVecScatter<uint32_t>((uint64_t)ShapeData.TriangleMesh.mTriangles, ShapeData.TriangleMesh.mNbTriangles * 3, &ShapeData.Indices);
            }
        } else if (ShapeData.mType == PxGeometryType::eHEIGHTFIELD) {
            const uint32_t nb = ShapeData.mHeightField.mNbSamples;
            AddReadVecScatter((uint64_t)ShapeData.mHeightField.mData.mSamples, nb, &ShapeData.mSamples);
        } else if (ShapeData.mType == PxGeometryType::eCONVEXMESH) {
            AddReadVecScatter<physx::PxVec3>((uint64_t)ShapeData.ConvexMesh.HullData.getHullVertices(), (size_t)ShapeData.ConvexMesh.HullData.HullVerticesNb, &ShapeData.Vertices);
        }
    }
    
    // 转换16位索引为32位
    for (auto& ptr : ShapeDataPtrs) {
        auto& ShapeData = *ptr;
        if (ShapeData.mType == PxGeometryType::eTRIANGLEMESH && !ShapeData.SmallIndices.empty()) {
            const size_t size = ShapeData.SmallIndices.size();
            ShapeData.Indices.resize(size);
            std::transform(
                ShapeData.SmallIndices.begin(),
                ShapeData.SmallIndices.end(),
                ShapeData.Indices.begin(),
                [](uint16_t val) { return static_cast<uint32_t>(val); }
            );
            ShapeData.SmallIndices.clear();
            ShapeData.SmallIndices.shrink_to_fit();
        }
    }
    
    // 生成三角形网格数据
    for (size_t i = 0; i < ShapeDataPtrs.size(); i++) {
        const auto& ShapeDataItem = *ShapeDataPtrs[i];
        physx::PxTransform GlobalPose{};
        
        if (isDynamic) {
            GlobalPose = ShapeDataItem.Actor.mRigid.mCore.mBodyToWorld * ShapeDataItem.Actor.mRigid.mBodyToActor.getInverse();
        } else {
            GlobalPose = ShapeDataItem.Actor.mRigid.mCore.mBodyToWorld;
        }
        
        auto LocalPose = ShapeDataItem.Shape.ShapeCore.mCore.transform;
        auto CombinePose = GlobalPose * LocalPose;
        auto UniqueKey = ShapeDataItem.UniqueKey;
        
        if (ShapeDataItem.mType == PxGeometryType::eBOX) {
            const PhysX::PxBoxGeometry& boxGeometry = (PhysX::PxBoxGeometry&)ShapeDataItem.Shape.GetGeometry();
            auto halfExtents = boxGeometry.mHalfExtents;
            
            physx::PxVec3 vertices[8];
            vertices[0] = physx::PxVec3(-halfExtents.x, -halfExtents.y, -halfExtents.z);
            vertices[1] = physx::PxVec3(halfExtents.x, -halfExtents.y, -halfExtents.z);
            vertices[2] = physx::PxVec3(halfExtents.x, halfExtents.y, -halfExtents.z);
            vertices[3] = physx::PxVec3(-halfExtents.x, halfExtents.y, -halfExtents.z);
            vertices[4] = physx::PxVec3(-halfExtents.x, -halfExtents.y, halfExtents.z);
            vertices[5] = physx::PxVec3(halfExtents.x, -halfExtents.y, halfExtents.z);
            vertices[6] = physx::PxVec3(halfExtents.x, halfExtents.y, halfExtents.z);
            vertices[7] = physx::PxVec3(-halfExtents.x, halfExtents.y, halfExtents.z);
            
            if (autoTransform) {
                for (int j = 0; j < 8; j++) {
                    vertices[j] = CombinePose.transform(vertices[j]);
                }
            }
            
            uint32_t indices[] = {
                0, 1, 2, 0, 2, 3,
                4, 6, 5, 4, 7, 6,
                0, 4, 5, 0, 5, 1,
                2, 6, 7, 2, 7, 3,
                0, 3, 7, 0, 7, 4,
                1, 5, 6, 1, 6, 2
            };
            
            TriangleMeshData mesh_data{};
            mesh_data.Vertices.assign(vertices, vertices + 8);
            mesh_data.Indices.assign(indices, indices + 36);
            mesh_data.Flags = ShapeDataItem.m_shape_flags;
            mesh_data.QueryFilterData = ShapeDataItem.Shape.ShapeCore.QueryFilterData;
            mesh_data.SimulationFilterData = ShapeDataItem.Shape.ShapeCore.SimulationFilterData;
            mesh_data.UniqueKey1 = UniqueKey;
            mesh_data.Type = PxGeometryType::eBOX;
            mesh_data.Transform = CombinePose;
            mesh_data.bIsVehicle = ShapeDataItem.bIsVehicleWheel;
            mesh_data.WheelRadius = ShapeDataItem.WheelRadius;
            mesh_data.WheelWidth = ShapeDataItem.WheelWidth;
            
            TriangleMeshDatas.push_back(mesh_data);
        } else if (ShapeDataItem.mType == PxGeometryType::eTRIANGLEMESH) {
            TriangleMeshData MeshData{};
            PhysX::PxMeshScale ScaleVal = ShapeDataItem.Scale;
            const size_t vertexCount = ShapeDataItem.Vertices.size();
            MeshData.Vertices.resize(vertexCount);
            
            for (size_t j = 0; j < vertexCount; j++) {
                physx::PxVec3& vertex = MeshData.Vertices[j];
                vertex = ScaleVal.transform(ShapeDataItem.Vertices[j]);
                if (autoTransform) {
                    vertex = CombinePose.transform(vertex);
                }
            }
            
            MeshData.Indices = ShapeDataItem.Indices;
            MeshData.Flags = ShapeDataItem.m_shape_flags;
            MeshData.QueryFilterData = ShapeDataItem.Shape.ShapeCore.QueryFilterData;
            MeshData.SimulationFilterData = ShapeDataItem.Shape.ShapeCore.SimulationFilterData;
            MeshData.UniqueKey1 = UniqueKey;
            MeshData.Type = PxGeometryType::eTRIANGLEMESH;
            MeshData.Transform = CombinePose;
            MeshData.bIsVehicle = ShapeDataItem.bIsVehicleWheel;
            MeshData.WheelRadius = ShapeDataItem.WheelRadius;
            MeshData.WheelWidth = ShapeDataItem.WheelWidth;
            
            TriangleMeshDatas.push_back(MeshData);
        } else if (ShapeDataItem.mType == PxGeometryType::eHEIGHTFIELD) {
            PhysX::PxHeightFieldGeometryT field_geometry = (PhysX::PxHeightFieldGeometryT&)ShapeDataItem.Shape.GetGeometry();
            
            auto NumRows = ShapeDataItem.mHeightField.mData.Rows;
            auto NumColumns = ShapeDataItem.mHeightField.mData.Columns;
            auto ColumnScale = field_geometry.ColumnScale;
            auto RowScale = field_geometry.RowScale;
            auto HeightScale = field_geometry.HeightScale;
            
            TriangleMeshData MeshData{};
            
            if (NumRows == 0 || NumColumns == 0 || ShapeDataItem.mSamples.empty()) {
                continue;
            }
            
            for (uint32_t row = 0; row < NumRows - 1; row++) {
                for (uint32_t col = 0; col < NumColumns - 1; col++) {
                    uint32_t idx00 = row * NumColumns + col;
                    uint32_t idx10 = idx00 + 1;
                    uint32_t idx01 = idx00 + NumColumns;
                    uint32_t idx11 = idx01 + 1;
                    
                    auto sample00 = ShapeDataItem.mSamples[idx00];
                    auto sample10 = ShapeDataItem.mSamples[idx10];
                    auto sample01 = ShapeDataItem.mSamples[idx01];
                    auto sample11 = ShapeDataItem.mSamples[idx11];
                    
                    physx::PxVec3 v00, v10, v01, v11;
                    v00 = {row * RowScale, sample00.mHeight * HeightScale, col * ColumnScale};
                    v10 = {row * RowScale, sample10.mHeight * HeightScale, (col + 1) * ColumnScale};
                    v01 = {(row + 1) * RowScale, sample01.mHeight * HeightScale, col * ColumnScale};
                    v11 = {(row + 1) * RowScale, sample11.mHeight * HeightScale, (col + 1) * ColumnScale};
                    
                    if (autoTransform) {
                        v00 = CombinePose.transform(v00);
                        v10 = CombinePose.transform(v10);
                        v01 = CombinePose.transform(v01);
                        v11 = CombinePose.transform(v11);
                    }
                    
                    MeshData.Vertices.push_back(v00);
                    MeshData.Vertices.push_back(v10);
                    MeshData.Vertices.push_back(v01);
                    MeshData.Vertices.push_back(v11);
                    
                    size_t base_idx = (row * (NumColumns - 1) + col) * 4;
                    
                    MeshData.Indices.push_back(base_idx);
                    MeshData.Indices.push_back(base_idx + 1);
                    MeshData.Indices.push_back(base_idx + 2);
                    
                    MeshData.Indices.push_back(base_idx + 1);
                    MeshData.Indices.push_back(base_idx + 3);
                    MeshData.Indices.push_back(base_idx + 2);
                }
            }
            
            MeshData.Flags = ShapeDataItem.m_shape_flags;
            MeshData.QueryFilterData = ShapeDataItem.Shape.ShapeCore.QueryFilterData;
            MeshData.SimulationFilterData = ShapeDataItem.Shape.ShapeCore.SimulationFilterData;
            MeshData.UniqueKey1 = UniqueKey;
            MeshData.UniqueKey2 = (uint64_t)((PhysX::PxHeightFieldGeometryT&)ShapeDataItem.Shape.GetGeometry()).mHeightField;
            MeshData.Type = PxGeometryType::eHEIGHTFIELD;
            MeshData.Transform = CombinePose;
            MeshData.bIsVehicle = ShapeDataItem.bIsVehicleWheel;
            MeshData.WheelRadius = ShapeDataItem.WheelRadius;
            MeshData.WheelWidth = ShapeDataItem.WheelWidth;
            
            TriangleMeshDatas.push_back(MeshData);
        } else if (ShapeDataItem.mType == PxGeometryType::eCONVEXMESH) {
            const PhysX::PxMeshScale ScaleVal = ShapeDataItem.Scale;
            TriangleMeshData MeshData{};
            MeshData.Vertices = ShapeDataItem.Vertices;
            const size_t vertexCount = MeshData.Vertices.size();
            
            for (size_t j = 0; j < vertexCount; j++) {
                if (autoTransform) {
                    MeshData.Vertices[j] = CombinePose.transform(ScaleVal.transform(MeshData.Vertices[j]));
                }
            }
            
            for (const auto& polygon : ShapeDataItem.polygons) {
                for (uint16_t j = 0; j < polygon.mNbVerts - 2; j++) {
                    uint32_t index_idx0 = polygon.mVRef8 + 0;
                    uint32_t index_idx1 = polygon.mVRef8 + j + 1;
                    uint32_t index_idx2 = polygon.mVRef8 + j + 2;
                    
                    if (index_idx0 >= ShapeDataItem.ConvexIndices.size() || index_idx2 >= ShapeDataItem.ConvexIndices.size())
                        continue;
                    
                    uint32_t idx0 = ShapeDataItem.ConvexIndices[index_idx0];
                    uint32_t idx1 = ShapeDataItem.ConvexIndices[index_idx1];
                    uint32_t idx2 = ShapeDataItem.ConvexIndices[index_idx2];
                    
                    MeshData.Indices.push_back(idx0);
                    MeshData.Indices.push_back(idx1);
                    MeshData.Indices.push_back(idx2);
                }
            }
            
            MeshData.Flags = ShapeDataItem.m_shape_flags;
            MeshData.QueryFilterData = ShapeDataItem.Shape.ShapeCore.QueryFilterData;
            MeshData.SimulationFilterData = ShapeDataItem.Shape.ShapeCore.SimulationFilterData;
            MeshData.UniqueKey1 = UniqueKey;
            MeshData.Type = PxGeometryType::eCONVEXMESH;
            MeshData.Transform = CombinePose;
            MeshData.bIsVehicle = ShapeDataItem.bIsVehicleWheel;
            MeshData.WheelRadius = ShapeDataItem.WheelRadius;
            MeshData.WheelWidth = ShapeDataItem.WheelWidth;
            
            TriangleMeshDatas.push_back(MeshData);
        }
    }
    
    return TriangleMeshDatas;
}

inline std::vector<TriangleMeshData> LoadDynamicRigidShape(
        std::set<PrunerPayload>& currentSceneObjects,
        std::unordered_map<PrunerPayload, physx::PxTransform, PrunerPayloadHash>& cache,
        std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash>& ptrCache,
        std::set<PrunerPayload>& willRemoveObjects
){
    std::vector<PrunerPayload> mObjects = CollectAllDynamicRigidActorShapes();
    std::set<uint64_t> queryActor{};
    for (const auto& obj : mObjects) {
        queryActor.insert(obj.Actor);
    }
    std::unordered_map<uint64_t, std::shared_ptr<PhysX::BodyT>> actorPos{};
    std::unordered_map<uint64_t, std::shared_ptr<PhysX::ShapeT>> shapePos{};
    actorPos.reserve(queryActor.size());
    shapePos.reserve(mObjects.size());

    for (const auto& actor : queryActor)
    {
        auto body = std::make_shared<PhysX::BodyT>();
        if (绘制.读写.read(actor, body.get(), sizeof(PhysX::BodyT))) {
             actorPos[actor] = body;
        }
    }

    for (const auto& obj : mObjects) {
        auto shape = std::make_shared<PhysX::ShapeT>();
        if (绘制.读写.read(obj.Shape, shape.get(), sizeof(PhysX::ShapeT))) {
             shapePos[obj.Shape] = shape;
        }
    }

    std::vector<PrunerPayload> filteredObjects;
    filteredObjects.reserve(mObjects.size());
    for (const auto& obj : mObjects) {
        auto shapeIt = shapePos.find(obj.Shape);
        if (shapeIt == shapePos.end()) {
            continue;
        }
        auto type = shapeIt->second->ShapeCore.getGeometryType();
        if (type == PxGeometryType::ePLANE) {
            currentSceneObjects.erase(obj);
            willRemoveObjects.insert(obj);
            cache.erase(obj);
            ptrCache.erase(obj);
            continue;
        }
        filteredObjects.push_back(obj);
    }
    mObjects.swap(filteredObjects);

    std::set<PrunerPayload> removeObjects{};
    std::vector<PrunerPayload> addObjects{};
    for (const auto& obj : currentSceneObjects) {
        auto shapeIt = shapePos.find(obj.Shape);
        if (shapeIt == shapePos.end()) {
            continue;
        }
        auto type = shapeIt->second->ShapeCore.getGeometryType();
        if (type == PxGeometryType::eCAPSULE || type == PxGeometryType::eSPHERE || type == PxGeometryType::ePLANE) {
            willRemoveObjects.insert(obj);
            removeObjects.insert(obj);
        }
    }
    for (const auto& obj : mObjects) {
        if (actorPos.find(obj.Actor) == actorPos.end() || shapePos.find(obj.Shape) == shapePos.end()) {
             continue;
        }
        auto& body = *actorPos[obj.Actor];
        auto& shape = *shapePos[obj.Shape];
        physx::PxTransform GlobalPose = (body.mRigid.mCore.mBodyToWorld * body.mRigid.mBodyToActor.getInverse()) * shape.ShapeCore.mCore.transform;

        auto it = cache.find(obj);
        if (it != cache.end()) {
            const float tolerance = 0.1f;

            const physx::PxTransform& a = it->second;
            const physx::PxTransform& b = GlobalPose;
            bool positionChanged =
                    std::abs(a.p.x - b.p.x) > tolerance ||
                    std::abs(a.p.y - b.p.y) > tolerance ||
                    std::abs(a.p.z - b.p.z) > tolerance;

            bool rotationChanged =
                    std::abs(a.q.x - b.q.x) > tolerance ||
                    std::abs(a.q.y - b.q.y) > tolerance ||
                    std::abs(a.q.z - b.q.z) > tolerance ||
                    std::abs(a.q.w - b.q.w) > tolerance;

            if (positionChanged || rotationChanged) {
                currentSceneObjects.erase(obj);
                willRemoveObjects.insert(obj);
            }
        }
        cache[obj] = GlobalPose;
        auto ptrIt = ptrCache.find(obj);
        const auto& geom = shape.GetGeometry();
        PhysX::PxTriangleMeshGeometryT tri_geom = reinterpret_cast<const PhysX::PxTriangleMeshGeometryT&>(geom);
        auto trianglePtr = (uint64_t)tri_geom.mTriangleMesh;
        if (ptrIt != ptrCache.end()) {
            if (ptrIt->second != trianglePtr) {
                currentSceneObjects.erase(obj);
                willRemoveObjects.insert(obj);
            }
        }
        ptrCache[obj] = trianglePtr;
    }

    std::set<PrunerPayload> mObjectsSet(mObjects.begin(), mObjects.end());
    for (auto it = cache.begin(); it != cache.end(); ) {
        if (mObjectsSet.find(it->first) == mObjectsSet.end()) {
            it = cache.erase(it);
        }
        else {
            ++it;
        }
    }

    for (auto it = ptrCache.begin(); it != ptrCache.end(); ) {
        if (mObjectsSet.find(it->first) == mObjectsSet.end()) {
            it = ptrCache.erase(it);
        }
        else {
            ++it;
        }
    }

    std::set<PrunerPayload> result;
    for (const auto& [obj, pos] : cache) {
        result.insert(obj);
    }
    std::copy_if(currentSceneObjects.begin(), currentSceneObjects.end(), std::inserter(removeObjects, removeObjects.begin()),[&result](const PrunerPayload& obj) {
        return result.find(obj) == result.end();
    });

    std::copy_if(result.begin(), result.end(), std::back_inserter(addObjects),[&currentSceneObjects](const PrunerPayload& obj) {
                     return currentSceneObjects.find(obj) == currentSceneObjects.end();
                 }
    );

    for (const auto& obj : removeObjects) {
        currentSceneObjects.erase(obj);
        willRemoveObjects.insert(obj);
    }

    for (const auto& obj : addObjects) {
        currentSceneObjects.insert(obj);
    }

    return GetMeshData(addObjects,
                       [](const PhysX::ShapeDataT& shape_data) {
                PxGeometryType type = shape_data.mType;
                uint8_t Flags = shape_data.m_shape_flags;
                if (type == PxGeometryType::eBOX || 
                    type == PxGeometryType::eCONVEXMESH || 
                    type == PxGeometryType::eTRIANGLEMESH) {
                    return !((Flags & 0xF) == 0x3 || (Flags & 0xF) == 0xB);
                }
                return true;
            },
                        true
    );
}

std::vector<TriangleMeshData> RefreshDynamicLoadHeightField(
    uint32_t& lastTimestamp,
    std::set<PrunerPayload>& UniqueKeySet,
    std::set<PrunerPayload>& HeightFieldSet,
    std::set<uint64_t>& HeightFieldSamplePtrSet,
    std::set<uint64_t>& RemoveHeightFieldKey
) {
    uint64_t PhysxInstancePtr = 绘制.读写.read<uint64_t>(g_libUE4 + PhysXPtr);
    auto px_scene_arr_ptr = 绘制.读写.read<uint64_t>(PhysxInstancePtr + 0x8);
    auto px_scene_ptr = 绘制.读写.read<uint64_t>(px_scene_arr_ptr);
    auto scene = 绘制.读写.read<PhysX::NpSceneT>(px_scene_ptr);
    
    if (scene.exts[0].mTimestamp == lastTimestamp) {
        return std::vector<TriangleMeshData>{};
    }
    
    lastTimestamp = scene.exts[0].mTimestamp;
    std::vector<PrunerPayload> mObjects = CollectAllHeightRigidActorShapes();
    std::set<PrunerPayload> newUnionKeySet(mObjects.begin(), mObjects.end());
    
    std::vector<PrunerPayload> newObjects;
    std::copy_if(mObjects.begin(), mObjects.end(), std::back_inserter(newObjects),
        [&UniqueKeySet, &HeightFieldSet](const PrunerPayload& obj) {
            return UniqueKeySet.find(obj) == UniqueKeySet.end() || HeightFieldSet.find(obj) != HeightFieldSet.end();
        });
    
    UniqueKeySet.insert(newObjects.begin(), newObjects.end());
    
    std::set<PrunerPayload> RemoveKey{};
    std::set_difference(UniqueKeySet.begin(), UniqueKeySet.end(),
        newUnionKeySet.begin(), newUnionKeySet.end(),
        std::inserter(RemoveKey, RemoveKey.begin()));
    
    for (const auto& key : RemoveKey) {
        UniqueKeySet.erase(key);
    }
    
    std::set<uint64_t> currentHeightFieldPtrSet{};
    
    auto result = GetMeshData(newObjects,
        [&HeightFieldSamplePtrSet, &currentHeightFieldPtrSet, &HeightFieldSet](const PhysX::ShapeDataT& shape_data) {
            PxGeometryType type = shape_data.mType;
            auto queryFlag = shape_data.Shape.ShapeCore.QueryFilterData;
            auto simFilter = shape_data.Shape.ShapeCore.SimulationFilterData;
            uint8_t Flags = shape_data.m_shape_flags;
            
            if (type == PxGeometryType::eHEIGHTFIELD) {
                PhysX::PxHeightFieldGeometryT field_geometry = (PhysX::PxHeightFieldGeometryT&)shape_data.Shape.GetGeometry();
                HeightFieldSet.insert(shape_data.UniqueKey);
                
                if (field_geometry.RowScale == 200.0f) {
                    auto ptr = (uint64_t)field_geometry.mHeightField;
                    currentHeightFieldPtrSet.insert(ptr);
                    return !HeightFieldSamplePtrSet.insert(ptr).second;
                }
                return true;
            }
            return true;
        }
    );
    
    std::set_difference(HeightFieldSamplePtrSet.begin(), HeightFieldSamplePtrSet.end(),
        currentHeightFieldPtrSet.begin(), currentHeightFieldPtrSet.end(),
        std::inserter(RemoveHeightFieldKey, RemoveHeightFieldKey.begin()));
    
    for (const auto& key : RemoveHeightFieldKey) {
        HeightFieldSamplePtrSet.erase(key);
    }
    
    return result;
}

std::vector<TriangleMeshData> LoadShapeByRange(
    uint32_t& lastTimestamp,
    std::unordered_map<PrunerPayload, physx::PxTransform, PrunerPayloadHash>& cache,
    std::set<PrunerPayload>& currentSceneObjects,
    std::set<PrunerPayload>& willRemoveObjects,
    std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash>& alwaysCheckShape
) {
    uint64_t PhysxInstancePtr = 绘制.读写.read<uint64_t>(g_libUE4 + PhysXPtr);
    auto px_scene_arr_ptr = 绘制.读写.read<uint64_t>(PhysxInstancePtr + 0x8);
    auto px_scene_ptr = 绘制.读写.read<uint64_t>(px_scene_arr_ptr);
    auto scene = 绘制.读写.read<PhysX::NpSceneT>(px_scene_ptr);
    
    if (scene.exts[0].mTimestamp == lastTimestamp) {
        return std::vector<TriangleMeshData>{};
    }
    
    lastTimestamp = scene.exts[0].mTimestamp;
    std::vector<PrunerPayload> mObjects = CollectAllStaticRigidActorShapes();
    std::set<PrunerPayload> queryObjects{};
    std::set<PrunerPayload> mObjectsSet(mObjects.begin(), mObjects.end());
    
    for (const auto& obj : mObjectsSet) {
        if (cache.find(obj) == cache.end()) {
            queryObjects.insert(obj);
        }
    }
    
    std::set<uint64_t> queryActor{};
    for (const auto& obj : queryObjects) {
        queryActor.insert(obj.Actor);
    }
    
    std::unordered_map<uint64_t, PhysX::BodyT> actorPos{};
    std::unordered_map<uint64_t, PhysX::ShapeT> shapePos{};
    actorPos.reserve(queryActor.size());
    shapePos.reserve(queryObjects.size());
    
    for (const auto& actor : queryActor) {
        actorPos[actor] = {};
        绘制.读写.read(actor, &actorPos[actor], sizeof(PhysX::BodyT));
    }
    
    for (const auto& obj : queryObjects) {
        shapePos[obj.Shape] = {};
        绘制.读写.read(obj.Shape, &shapePos[obj.Shape], sizeof(PhysX::ShapeT));
    }
    
    for (const auto& obj : queryObjects) {
        auto& body = actorPos[obj.Actor];
        auto& shape = shapePos[obj.Shape];
        physx::PxTransform GlobalPose = body.mRigid.mCore.mBodyToWorld * shape.ShapeCore.mCore.transform;
        cache[obj] = GlobalPose;
    }
    
    std::set<PrunerPayload> removeObjects{};
    std::vector<PrunerPayload> addObjects{};
    
    for (auto it = cache.begin(); it != cache.end(); ) {
        if (mObjectsSet.find(it->first) == mObjectsSet.end()) {
            it = cache.erase(it);
        } else {
            ++it;
        }
    }
    
    std::set<PrunerPayload> result;
    for (const auto& [obj, pos] : cache) {
        result.insert(obj);
    }
    
    std::copy_if(currentSceneObjects.begin(), currentSceneObjects.end(),
        std::inserter(removeObjects, removeObjects.begin()),
        [&result](const PrunerPayload& obj) {
            return result.find(obj) == result.end();
        });
    
    std::copy_if(result.begin(), result.end(), std::back_inserter(addObjects),
        [&currentSceneObjects, &alwaysCheckShape](const PrunerPayload& obj) {
            return currentSceneObjects.find(obj) == currentSceneObjects.end() ||
                alwaysCheckShape.find(obj) != alwaysCheckShape.end();
        });
    
    for (const auto& obj : removeObjects) {
        currentSceneObjects.erase(obj);
        alwaysCheckShape.erase(obj);
        willRemoveObjects.insert(obj);
    }
    
    for (const auto& obj : addObjects) {
        currentSceneObjects.insert(obj);
    }
    
    auto res = GetMeshData(addObjects,
        [&alwaysCheckShape, &willRemoveObjects, &currentSceneObjects](const PhysX::ShapeDataT& shape_data) {
            PxGeometryType type = shape_data.mType;
            uint8_t Flags = shape_data.m_shape_flags;
            uint8_t masked = Flags & 0xF;
            
            if (type != PxGeometryType::eTRIANGLEMESH && type != PxGeometryType::eBOX && type != PxGeometryType::eCONVEXMESH) {
                willRemoveObjects.insert(shape_data.UniqueKey);
                currentSceneObjects.erase(shape_data.UniqueKey);
                return true;
            }
            
            auto findRes = alwaysCheckShape.find(shape_data.UniqueKey);
            auto trianglePtr = (uint64_t)((PhysX::PxTriangleMeshGeometryT&)shape_data.Shape.GetGeometry()).mTriangleMesh;
            
            if (findRes == alwaysCheckShape.end()) {
                alwaysCheckShape.insert({shape_data.UniqueKey, trianglePtr});
            } else {
                if (findRes->second == trianglePtr) {
                    return true;
                } else {
                    alwaysCheckShape[shape_data.UniqueKey] = trianglePtr;
                    willRemoveObjects.insert(shape_data.UniqueKey);
                    currentSceneObjects.erase(shape_data.UniqueKey);
                    return false;
                }
            }
            
            return !((type == PxGeometryType::eTRIANGLEMESH || type == PxGeometryType::eBOX || type == PxGeometryType::eCONVEXMESH) &&
                ((Flags & 0xF) == 0x3 || (Flags & 0xF) == 0xB));
        }
    );
    
    return res;
}

void Physics::embreeErrorFunction(void* userPtr, RTCError code, const char* str) {
    printf("[DEBUG] Embree Error [%d]: %s \n", code, str);
}

template <typename T, typename Hash>
const std::vector<std::shared_ptr<TriangleMeshData>>& Physics::VisibleScene<T, Hash>::GetMeshDatas() const {
    return mesh_datas;
}

template <typename T, typename Hash>
Physics::VisibleScene<T, Hash>::VisibleScene(KeyExtractor keyExtractor) : getKey(keyExtractor) {
    this->device = rtcNewDevice(nullptr);
    rtcSetDeviceErrorFunction(device, embreeErrorFunction, nullptr);
    this->scene = rtcNewScene(device);
    rtcSetSceneBuildQuality(scene, RTC_BUILD_QUALITY_LOW);
    rtcSetSceneFlags(scene, RTC_SCENE_FLAG_DYNAMIC);
    rtcCommitScene(this->scene);
}

template <typename T, typename Hash>
Physics::VisibleScene<T, Hash>::~VisibleScene() {
    mesh_datas.clear();
    
    if (scene) {
        rtcReleaseScene(scene);
        scene = nullptr;
    }
    
    if (device) {
        rtcReleaseDevice(device);
        device = nullptr;
    }
}

template <typename T, typename Hash>
void Physics::VisibleScene<T, Hash>::UpdateMesh(
    const std::vector<TriangleMeshData>& willAddMeshs,
    const std::set<T>& RemoveKey
) {
    for (auto& key : RemoveKey) {
        if (geometry_id_map.find(key) != geometry_id_map.end()) {
            auto geometry_id = geometry_id_map[key];
            auto geometry = rtcGetGeometry(scene, geometry_id);
            rtcDisableGeometry(geometry);
            disabled_geometry_ids.insert(geometry_id);
            geometry_id_map.erase(key);
        }
    }
    
    if (!mesh_datas.empty()) {
        mesh_datas.erase(
            std::remove_if(
                mesh_datas.begin(), mesh_datas.end(),
                [this, &RemoveKey](const std::shared_ptr<TriangleMeshData>& mesh) {
                    return RemoveKey.find(this->getKey(*mesh)) != RemoveKey.end();
                }
            ),
            mesh_datas.end()
        );
    }
    
    for (auto& mesh : willAddMeshs) {
        if (mesh.Vertices.empty() || mesh.Indices.empty()) {
            continue;
        }
        
        RTCGeometry geom;
        bool should_release = false;
        uint32_t geometry_id = 0;
        auto mesh_copy = std::make_shared<TriangleMeshData>(mesh);
        mesh_datas.push_back(mesh_copy);
        
        if (!disabled_geometry_ids.empty()) {
            geometry_id = *disabled_geometry_ids.begin();
            disabled_geometry_ids.erase(disabled_geometry_ids.begin());
            geom = rtcGetGeometry(scene, geometry_id);
            rtcEnableGeometry(geom);
        } else {
            geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
            should_release = true;
        }
        
        float* vertices = (float*)rtcSetNewGeometryBuffer(geom,
            RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3,
            3 * sizeof(float), mesh.Vertices.size());
        
        if (!vertices) {
            printf("Error: Failed to allocate vertex buffer\n");
            if (should_release) {
                rtcReleaseGeometry(geom);
            } else {
                rtcDisableGeometry(geom);
                disabled_geometry_ids.insert(geometry_id);
            }
            break;
        }
        
        for (size_t i = 0; i < mesh.Vertices.size(); i++) {
            vertices[i * 3] = mesh.Vertices[i].x;
            vertices[i * 3 + 1] = mesh.Vertices[i].y;
            vertices[i * 3 + 2] = mesh.Vertices[i].z;
        }
        
        auto bufferSize = mesh.Indices.size() / 3;
        unsigned int* indices = (unsigned int*)rtcSetNewGeometryBuffer(geom,
            RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3,
            3 * sizeof(unsigned int), bufferSize);
        
        if (!indices) {
            printf("Error: Failed to allocate index buffer\n");
            if (should_release) {
                rtcReleaseGeometry(geom);
            } else {
                rtcDisableGeometry(geom);
                disabled_geometry_ids.insert(geometry_id);
            }
            break;
        }
        
        memcpy(indices, mesh.Indices.data(), mesh.Indices.size() * sizeof(uint32_t));
        rtcSetGeometryUserData(geom, mesh_copy.get());
        rtcCommitGeometry(geom);
        
        if (should_release) {
            try {
                geometry_id = rtcAttachGeometry(scene, geom);
            } catch (...) {
                printf("Error attach geom.\n");
                rtcReleaseGeometry(geom);
                return;
            }
            rtcReleaseGeometry(geom);
        }
        
        T key = getKey(mesh);
        geometry_id_map.insert({key, geometry_id});
    }
    
    rtcCommitScene(scene);
}

template <typename T, typename Hash>
RTCRayHit Physics::VisibleScene<T, Hash>::Raycast(physx::PxVec3& origin, physx::PxVec3& target) {
    try {
        RTCRay ray;
        ray.org_x = origin.x;
        ray.org_y = origin.y;
        ray.org_z = origin.z;
        ray.dir_x = target.x - origin.x;
        ray.dir_y = target.y - origin.y;
        ray.dir_z = target.z - origin.z;
        
        float dir_length = std::sqrt(ray.dir_x * ray.dir_x + ray.dir_y * ray.dir_y + ray.dir_z * ray.dir_z);
        ray.dir_x /= dir_length;
        ray.dir_y /= dir_length;
        ray.dir_z /= dir_length;
        
        ray.tnear = 0.0f;
        ray.tfar = dir_length;
        ray.mask = -1;
        ray.flags = 0;
        
        RTCRayHit rayhit;
        rayhit.ray = ray;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        
        if (this->scene) {
            rtcIntersect1(this->scene, &rayhit, nullptr);
        }
        
        return rayhit;
    } catch (...) {
        printf("Raycast error\n");
        RTCRayHit rayhit;
        rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
        return rayhit;
    }
}

template <typename T, typename Hash>
TriangleMeshData* Physics::VisibleScene<T, Hash>::GetGeomeoryData(uint32_t geomId) {
    return (TriangleMeshData*)rtcGetGeometryUserData(rtcGetGeometry(scene, geomId));
}

Throttler::Throttler() {}

void Throttler::executeTaskWithSleep(const std::string& taskName,
                                     std::chrono::duration<double> interval,
                                     const std::function<void()>& task) {
    using Clock = std::chrono::steady_clock;
    auto now = Clock::now();
    
    {
        auto it = lastExecuted_.find(taskName);
        if (it != lastExecuted_.end()) {
            auto elapsed = now - it->second;
            if (elapsed < interval) {
                std::this_thread::sleep_for(interval - elapsed);
            }
        }
        lastExecuted_[taskName] = Clock::now();
    }
    
    task();
}

void VisibleCheck::UpdateSceneByRange() {
    std::unordered_map<PrunerPayload, physx::PxTransform, PrunerPayloadHash> cache{};
    std::set<PrunerPayload> currentSceneObjects{};
    std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash> alwaysCheckShape{};
    uint32_t lastUpdateTimestamp = 0;
    Throttler Throttlered;
    std::vector<TriangleMeshData> pendingMeshs;
    std::set<PrunerPayload> pendingRemove;
    
    while (true) {
        Throttlered.executeTaskWithSleep("UpdateSceneByRangeSleep", std::chrono::milliseconds(800),
            [&] {
                std::set<PrunerPayload> willRemoveObjects{};
                auto Meshs = LoadShapeByRange(lastUpdateTimestamp, cache, currentSceneObjects,
                    willRemoveObjects, alwaysCheckShape);
                
                {
                    std::lock_guard<std::mutex> lock(Physics::sceneMutex);
                    if (!Meshs.empty() || !willRemoveObjects.empty()) {
                        for (const auto& mesh : Meshs) {
                            pendingMeshs.push_back(mesh);
                        }
                        for (const auto& remove : willRemoveObjects) {
                            pendingRemove.insert(remove);
                        }
                    }
                }
            });
        
        {
            std::lock_guard<std::mutex> lock(Physics::sceneMutex);
            if (!pendingMeshs.empty() || !pendingRemove.empty()) {
                DynamicLoadScene->UpdateMesh(pendingMeshs, pendingRemove);
                pendingMeshs.clear();
                pendingRemove.clear();
            }
        }
    }
}

void VisibleCheck::UpdateDynamicHeightField() {
    std::set<PrunerPayload> UniqueSet{};
    std::set<PrunerPayload> HeightFieldSet{};
    std::set<uint64_t> HeightFieldSamplePtrSet{};
    uint32_t lastUpdateTimestamp = 0;
    Throttler Throttlered;
    std::vector<TriangleMeshData> pendingMeshs;
    std::set<uint64_t> pendingRemove;
    
    while (true) {
        Throttlered.executeTaskWithSleep("HeightFieldUpdateSleep", std::chrono::milliseconds(1200),
            [&] {
                std::set<uint64_t> RemoveHeightFieldKey{};
                auto Meshs = RefreshDynamicLoadHeightField(lastUpdateTimestamp, UniqueSet,
                    HeightFieldSet, HeightFieldSamplePtrSet, RemoveHeightFieldKey);
                
                {
                    std::lock_guard<std::mutex> lock(Physics::sceneMutex);
                    if (!Meshs.empty() || !RemoveHeightFieldKey.empty()) {
                        for (const auto& mesh : Meshs) {
                            pendingMeshs.push_back(mesh);
                        }
                        for (const auto& remove : RemoveHeightFieldKey) {
                            pendingRemove.insert(remove);
                        }
                    }
                }
            });
        
        {
            std::lock_guard<std::mutex> lock(Physics::sceneMutex);
            if (!pendingMeshs.empty() || !pendingRemove.empty()) {
                HeightFieldScene->UpdateMesh(pendingMeshs, pendingRemove);
                pendingMeshs.clear();
                pendingRemove.clear();
            }
        }
    }
}

void VisibleCheck::UpdateDynamicRigid() {
    Throttler Throttlered;
    std::unordered_map<PrunerPayload, physx::PxTransform, PrunerPayloadHash> cache{};
    std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash> ptrCache{};
    std::set<PrunerPayload> currentSceneObjects{};
    std::vector<TriangleMeshData> pendingMeshs;
    std::set<PrunerPayload> pendingRemove;
    
    while (true) {
        Throttlered.executeTaskWithSleep("DynamicRigidUpdateSleep", std::chrono::milliseconds(350),
            [&] {
                std::set<PrunerPayload> willRemoveShape{};
                
                auto Meshs = LoadDynamicRigidShape(currentSceneObjects, cache, ptrCache, willRemoveShape);
                
                {
                    std::lock_guard<std::mutex> lock(Physics::sceneMutex);
                    if (!Meshs.empty() || !willRemoveShape.empty()) {
                        for (const auto& mesh : Meshs) {
                            pendingMeshs.push_back(mesh);
                        }
                        for (const auto& remove : willRemoveShape) {
                            pendingRemove.insert(remove);
                        }
                    }
                }
            });
        
        {
            std::lock_guard<std::mutex> lock(Physics::sceneMutex);
            if (!pendingMeshs.empty() || !pendingRemove.empty()) {
                DynamicRigidScene->UpdateMesh(pendingMeshs, pendingRemove);
                pendingMeshs.clear();
                pendingRemove.clear();
            }
        }
    }
}

namespace LineTrace {
    bool initPhysX() {
        DynamicLoadScene = new Physics::VisibleScene<PrunerPayload, PrunerPayloadHash>(Physics::prunerPayloadExtractor);
        DynamicRigidScene = new Physics::VisibleScene<PrunerPayload, PrunerPayloadHash>(Physics::prunerPayloadExtractor);
        HeightFieldScene = new Physics::VisibleScene<uint64_t, Int64Hash>(Physics::int64Extractor);
        
        if (DynamicLoadScene && DynamicRigidScene && HeightFieldScene) {
            return true;
        }
        
        std::cout << "[Error] PhysX initialization error" << std::endl;
        return false;
    }
    
    physx::PxVec3 ToPx(float x, float y, float z) {
        return physx::PxVec3(x, y, z);
    }
    
    bool LineTraceSingle(const physx::PxVec3& Location, const physx::PxVec3& TraceEnd) {
        if (DynamicLoadScene == nullptr || HeightFieldScene == nullptr || DynamicRigidScene == nullptr) {
            return false;
        }
        
        physx::PxVec3 origin = Location;
        physx::PxVec3 target = TraceEnd;
        
        auto dynamicRayHit = DynamicLoadScene->Raycast(origin, target);
        if (dynamicRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID && dynamicRayHit.ray.tfar > 0.1f) {
            return true;
        }
        
        auto heightFieldRayHit = HeightFieldScene->Raycast(origin, target);
        if (heightFieldRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID && heightFieldRayHit.ray.tfar > 0.1f) {
            return true;
        }
        
        auto globalSceneRayHit = DynamicRigidScene->Raycast(origin, target);
        if (globalSceneRayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID && globalSceneRayHit.ray.tfar > 0.1f) {
            return true;
        }
        
        return false;
    }
    
    bool IsVehicleWheel(const physx::PxVec3& Location, const physx::PxVec3& TraceEnd) {
        if (DynamicRigidScene == nullptr) {
            return false;
        }
        
        physx::PxVec3 origin = Location;
        physx::PxVec3 target = TraceEnd;
        
        auto rayHit = DynamicRigidScene->Raycast(origin, target);
        if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID && rayHit.ray.tfar > 0.1f) {
            auto meshData = DynamicRigidScene->GetGeomeoryData(rayHit.hit.geomID);
            if (meshData && meshData->bIsVehicle) {
                return true;
            }
        }
        
        return false;
    }
}

Physics::VisibleScene<PrunerPayload, PrunerPayloadHash>* DynamicLoadScene = nullptr;
Physics::VisibleScene<PrunerPayload, PrunerPayloadHash>* DynamicRigidScene = nullptr;
Physics::VisibleScene<uint64_t, Int64Hash>* HeightFieldScene = nullptr;

template void AddReadVecScatter<float>(uint64_t address, size_t size, std::vector<float>* vec);
template void AddReadVecScatter<physx::PxVec3>(uint64_t address, size_t size, std::vector<physx::PxVec3>* vec);
template void AddReadVecScatter<uint8_t>(uint64_t address, size_t size, std::vector<uint8_t>* vec);
template void AddReadVecScatter<uint16_t>(uint64_t address, size_t size, std::vector<uint16_t>* vec);
template void AddReadVecScatter<uint32_t>(uint64_t address, size_t size, std::vector<uint32_t>* vec);
template void AddReadVecScatter<PhysX::PxHeightFieldSampleT>(uint64_t address, size_t size, std::vector<PhysX::PxHeightFieldSampleT>* vec);
template void AddReadVecScatter<PhysX::HullPolygonDataT>(uint64_t address, size_t size, std::vector<PhysX::HullPolygonDataT>* vec);

template class PhysX::PxBitAndDataT<unsigned char, 0x80>;
template class PhysX::PxBitAndDataT<unsigned short, 0x8000>;
template class PhysX::PxBitAndDataT<unsigned int, 0x80000000>;
template struct PhysX::PxPadding<3>;

template class Physics::VisibleScene<PrunerPayload, PrunerPayloadHash>;
template class Physics::VisibleScene<uint64_t, Int64Hash>;