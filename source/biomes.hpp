#pragma once

#include "block.hpp"

enum class DecorationFlags {
    None = 0x00,
    Oak = 0x01,
    CherryBlossom = 0x02,
    YellowFlower = 0x04,
    RedFlower = 0x08,
    Cactus = 0x10,
    DeadBush = 0x20,
};

inline DecorationFlags operator|(DecorationFlags a, DecorationFlags b) {
    return DecorationFlags((int)a | (int)b);
}

inline int operator&(DecorationFlags a, DecorationFlags b) {
    return (int)a & (int)b;
}

enum class BiomeID {
    Plains,
    Forest,
    Desert,
    Canyon,
};

struct Biome {
    double terrainBias; // The higher the value the more flat the terrain is
    double terrainYOffset; // Higher values will make the biome deeper, lower values will make it taller
    BlockType surfaceBlockTy;
    BlockType closeToSurfaceBlocksTy;
    DecorationFlags decorations;
    u16 oakChance;
    u16 cherryBlossomChance;
    u16 yellowFlowerChance;
    u16 redFlowerChance;
    u16 cactusChance;
    u16 deadBushChance;
};

const Biome biomes[4] = {
    Biome{
        .terrainBias = 6.0,
        .terrainYOffset = 0.0,
        .surfaceBlockTy = BlockType::Grass,
        .closeToSurfaceBlocksTy = BlockType::Dirt,
        .decorations = DecorationFlags::Oak | DecorationFlags::YellowFlower | DecorationFlags::RedFlower,
        .oakChance = 512,
        .yellowFlowerChance = 32,
        .redFlowerChance = 256,
    },
    Biome{
        .terrainBias = 1.5,
        .terrainYOffset = 0.0,
        .surfaceBlockTy = BlockType::Grass,
        .closeToSurfaceBlocksTy = BlockType::Dirt,
        .decorations = DecorationFlags::Oak | DecorationFlags::CherryBlossom | DecorationFlags::RedFlower,
        .oakChance = 96,
        .cherryBlossomChance = 512,
        .redFlowerChance = 256,
    },
    Biome{
        .terrainBias = 4.5,
        .terrainYOffset = 0.0,
        .surfaceBlockTy = BlockType::Sand,
        .closeToSurfaceBlocksTy = BlockType::Sand,
        .decorations = DecorationFlags::Cactus | DecorationFlags::DeadBush,
        .cactusChance = 256,
        .deadBushChance = 512,
    },
    Biome{
        .terrainBias = 1.0,
        .terrainYOffset = 0.15,
        .surfaceBlockTy = BlockType::Sand,
        .closeToSurfaceBlocksTy = BlockType::Sand,
        .decorations = DecorationFlags::DeadBush,
        .redFlowerChance = 156,
        .deadBushChance = 128,
    },
};

//  -------- > humidity
// |
// |
// |
// \/
// temperature
const int TEMPERATURE_LEVELS = 2;
const int HUMIDITY_LEVELS = 2;
const BiomeID biomeMap[TEMPERATURE_LEVELS][HUMIDITY_LEVELS] = {
    {BiomeID::Plains, BiomeID::Forest},
    {BiomeID::Desert, BiomeID::Canyon},
};

// Both temperature and humidity must be in the range of 0...1
inline const Biome& getBiome(double temperature, double humidity) {
    int temperatureIndex = clamp(int(temperature * TEMPERATURE_LEVELS), 0, TEMPERATURE_LEVELS - 1);
    int humidityIndex = clamp(int(humidity * HUMIDITY_LEVELS), 0, HUMIDITY_LEVELS - 1);
    //if ((temperatureIndex != 0 && temperatureIndex != 1) || (humidityIndex != 0 && humidityIndex != 1))
    //    std::cout << temperatureIndex << " : " << humidityIndex << std::endl;

    return biomes[(int)biomeMap[temperatureIndex][humidityIndex]];
}

// Uses bilinear interpolation
inline void getBiomeProperties(double temperature, double humidity, double& terrainBias, double& terrainYOffset) {
    double2 coord = double2(humidity * HUMIDITY_LEVELS, temperature * TEMPERATURE_LEVELS);
    int temperatureIndex = int(coord.y - 0.5);
    int humidityIndex = int(coord.x - 0.5);

    terrainBias = 0.0;
    terrainYOffset = 0.0;

    double totalFactor = 0.0;
    for (uint8_t y = 0; y < 2; y++) {
        for (uint8_t x = 0; x < 2; x++) {
            int xIndex = humidityIndex + x;
            int yIndex = temperatureIndex + y;
            if (xIndex >= 0 && xIndex < HUMIDITY_LEVELS && yIndex >= 0 && yIndex < TEMPERATURE_LEVELS) {
                // We negate x and y, since we need the area at the opposite of the square
                double2 oppositeBiomeCenter = double2((humidityIndex + !x) + 0.5, (temperatureIndex + !y) + 0.5);
                double area = abs(oppositeBiomeCenter.x - coord.x) * abs(oppositeBiomeCenter.y - coord.y);
                double factor = area * area;
                /*
                if (area > 1.0) {
                    std::cout << "WEIRD AREA: " << area << ", opposite biome center coord: " << oppositeBiomeCenter.x << ", " << oppositeBiomeCenter.y << ", temperature: " << temperature << ", humidity: " << humidity << std::endl;
                    std::cout << "Y: " << temperature * TEMPERATURE_LEVELS - 0.5 << " : " << temperatureIndex << std::endl;
                    std::cout << "X: " << humidity * HUMIDITY_LEVELS - 0.5 << " : " << humidityIndex << std::endl;
                }
                */
                //if (rand() % 1000 == 0)
                //    std::cout << "AREA: " << area << ", Y: " << yIndex << ", X: " << xIndex << std::endl;

                totalFactor += factor;

                const Biome& biome = biomes[(int)biomeMap[yIndex][xIndex]];
                terrainBias += biome.terrainBias * factor;
                terrainYOffset += biome.terrainYOffset * factor;
            }
        }
    }

    //if (rand() % 1000 == 0)
    //    std::cout << "BIAS: " << terrainBias / totalArea << ", " << "Total area: " << totalArea << std::endl;

    terrainBias /= totalFactor;
    terrainYOffset /= totalFactor;
}
