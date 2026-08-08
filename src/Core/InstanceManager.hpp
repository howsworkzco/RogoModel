#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>

// --- Minimalist Data Types ---
// Light components to maximize CPU cache alignment
struct Vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct Color3 {
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
};

// --- SoA (Structure of Arrays) Block ---
// All matching properties sit side-by-side in memory.
// This allows the GPU to stream these arrays instantly in one batch.
struct PartDataArrays {
    std::vector<uint32_t> instance_ids;
    std::vector<Vector3> positions;
    std::vector<Vector3> sizes;
    std::vector<Color3> colors;
};

// --- Data-Oriented Engine Registry ---
// Handles lightning-fast O(1) part lookups without relying on real objects.
class InstanceRegistry {
private:
    PartDataArrays m_PartStorage;
    
    // Quick lookup table: Maps [Instance ID] -> [Index inside m_PartStorage arrays]
    std::unordered_map<uint32_t, size_t> m_IdToIndex; 
    
    uint32_t m_NextId = 1;

public:
    InstanceRegistry() = default;
    ~InstanceRegistry() = default;

    // Creates an abstract part entry inside our flat arrays and returns its ID handle
    uint32_t CreatePart(Vector3 pos, Vector3 size, Color3 col) {
        uint32_t id = m_NextId++;
        size_t index = m_PartStorage.instance_ids.size();

        m_IdToIndex[id] = index;
        m_PartStorage.instance_ids.push_back(id);
        m_PartStorage.positions.push_back(pos);
        m_PartStorage.sizes.push_back(size);
        m_PartStorage.colors.push_back(col);

        return id;
    }

    // Direct O(1) position setter called by Luau
    void SetPosition(uint32_t id, Vector3 new_pos) {
        auto it = m_IdToIndex.find(id);
        if (it != m_IdToIndex.end()) {
            m_PartStorage.positions[it->second] = new_pos;
        } else {
            throw std::runtime_error("RogoModel Error: Attempted to set position on non-existent Instance ID.");
        }
    }

    // Direct O(1) position getter called by Luau
    Vector3 GetPosition(uint32_t id) const {
        auto it = m_IdToIndex.find(id);
        if (it != m_IdToIndex.end()) {
            return m_PartStorage.positions[it->second];
        }
        throw std::runtime_error("RogoModel Error: Attempted to get position from non-existent Instance ID.");
    }

    // Expose raw vectors directly to your graphics loop for hardware instancing
    const PartDataArrays& GetRawData() const { 
        return m_PartStorage; 
    }
};
