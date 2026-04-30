#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <set>
#include <memory>
#include <functional>
#include <chrono>
#include "foundation/PxTransform.h"
#include "foundation/PxVec3.h"
#include "rtcore.h"
#include <algorithm>
#include <memory>
#include <iostream>
#include <thread>
#include <numeric>
#include <sstream>
using namespace std;

namespace physx {
    struct PxVec3;
    struct PxTransform;
}

namespace PhysX {
    struct PxHeightFieldSampleT;
    struct HullPolygonDataT;
    
    template <typename storageType, storageType bitMask>
    class PxBitAndDataT;
    
    template<uint8_t TNumBytes>
    struct PxPadding;
}

enum class PxGeometryType : int32_t {
    eSPHERE        = 0,
    ePLANE         = 1,
    eCAPSULE       = 2,
    eBOX           = 3,
    eCONVEXMESH    = 4,
    eTRIANGLEMESH  = 5,
    eHEIGHTFIELD   = 6,
    eGEOMETRY_COUNT,
    eINVALID = -1
};

struct FilterDataT {
    uint32_t word0;
    uint32_t word1;
    uint32_t word2;
    uint32_t word3;
};

struct PrunerPayload {
    uint64_t Shape;
    uint64_t Actor;
    bool bIsVehicle;
    
    bool operator==(const PrunerPayload& other) const {
        return Shape == other.Shape && Actor == other.Actor;
    }
    
    bool operator<(const PrunerPayload& other) const {
        return Shape == other.Shape ? Actor < other.Actor : Shape < other.Shape;
    }
};

struct PrunerPayloadHash {
    size_t operator()(const PrunerPayload& p) const {
        return std::hash<uint64_t>()(p.Shape) ^ (std::hash<uint64_t>()(p.Actor) << 1);
    }
};

struct Int64Hash {
    size_t operator()(const uint64_t& p) const {
        return std::hash<uint64_t>()(p);
    }
};

struct TriangleMeshData {
    std::vector<physx::PxVec3> Vertices{};
    std::vector<uint32_t> Indices{};
    uint8_t Flags{};
    FilterDataT QueryFilterData{};
    FilterDataT SimulationFilterData{};
    PrunerPayload UniqueKey1;
    uint64_t UniqueKey2;
    PrunerPayload UniqueKey3;
    PxGeometryType Type{};
    physx::PxTransform Transform;
    bool bIsVehicle;
    float WheelRadius;
    float WheelWidth;
};

namespace Physics {
    extern std::mutex sceneMutex;

    inline auto prunerPayloadExtractor = [](const TriangleMeshData& mesh) -> PrunerPayload {
        return mesh.UniqueKey1;
    };
    
    inline auto int64Extractor = [](const TriangleMeshData& mesh) -> uint64_t {
        return mesh.UniqueKey2;
    };
    
    inline void embreeErrorFunction(void* userPtr, RTCError code, const char* str);

    template <typename T, typename Hash>
    class VisibleScene {
    public:
        using KeyExtractor = std::function<T(const TriangleMeshData&)>;
        
        VisibleScene(KeyExtractor keyExtractor);
        ~VisibleScene();
        
        const std::vector<std::shared_ptr<TriangleMeshData>>& GetMeshDatas() const;
        void UpdateMesh(
            const std::vector<TriangleMeshData>& willAddMeshs,
            const std::set<T>& RemoveKey
        );
        RTCRayHit Raycast(physx::PxVec3& origin, physx::PxVec3& target);
        TriangleMeshData* GetGeomeoryData(uint32_t geomId);
        
    private:
        RTCDevice device = nullptr;
        RTCScene scene = nullptr;
        std::unordered_map<T, uint32_t, Hash> geometry_id_map;
        std::set<uint32_t> disabled_geometry_ids;
        std::vector<std::shared_ptr<TriangleMeshData>> mesh_datas;
        KeyExtractor getKey;
    };
}

class Throttler {
public:
    Throttler();
    void executeTaskWithSleep(const std::string& taskName,
                              std::chrono::duration<double> interval,
                              const std::function<void()>& task);
private:
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> lastExecuted_;
};


void initue4(uint64_t libue4);

std::vector<TriangleMeshData> LoadDynamicRigidShape(
    std::set<PrunerPayload>& currentSceneObjects,
    std::unordered_map<PrunerPayload, physx::PxTransform, PrunerPayloadHash>& cache,
    std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash>& ptrCache,
    std::set<PrunerPayload>& willRemoveObjects
);

std::vector<TriangleMeshData> RefreshDynamicLoadHeightField(
    uint32_t& lastTimestamp,
    std::set<PrunerPayload>& UniqueKeySet,
    std::set<PrunerPayload>& HeightFieldSet,
    std::set<uint64_t>& HeightFieldSamplePtrSet,
    std::set<uint64_t>& RemoveHeightFieldKey
);

std::vector<TriangleMeshData> LoadShapeByRange(
    uint32_t& lastTimestamp,
    std::unordered_map<PrunerPayload, physx::PxTransform, PrunerPayloadHash>& cache,
    std::set<PrunerPayload>& currentSceneObjects,
    std::set<PrunerPayload>& willRemoveObjects,
    std::unordered_map<PrunerPayload, uint64_t, PrunerPayloadHash>& alwaysCheckShape
);

namespace VisibleCheck {
    void UpdateSceneByRange();
    void UpdateDynamicHeightField();
    void UpdateDynamicRigid();
}

namespace LineTrace {
    bool initPhysX();
    bool LineTraceSingle(const physx::PxVec3& Location, const physx::PxVec3& TraceEnd);
    physx::PxVec3 ToPx(float x, float y, float z);
}

extern Physics::VisibleScene<PrunerPayload, PrunerPayloadHash>* DynamicLoadScene;
extern Physics::VisibleScene<PrunerPayload, PrunerPayloadHash>* DynamicRigidScene;
extern Physics::VisibleScene<uint64_t, Int64Hash>* HeightFieldScene;