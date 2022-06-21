#include <tuple>
#include <cmath>
#include <vector>
#include <string>
#include <random>

#include <SFML/Graphics.hpp>

sf::Vector2f
normalize(const sf::Vector2f source);

int
wrapAroundIncrementedByOne(int dimension, int value);

float
smoothStep(float low, float high, float value);

float
dotProduct(const sf::Vector2f& a, const sf::Vector2f& b);

float
vectorLength(const sf::Vector2f& source);

struct Perlin {
    public: explicit
    Perlin(short seed, unsigned short squareGridSize, unsigned short gridSubdivisions)
        : squareGridSize(squareGridSize)
        , gridSubdivisions(gridSubdivisions)
    {
        auto        engine            = std::mt19937_64(seed);
        const auto  vectorsToGenerate = squareGridSize * squareGridSize;
        auto        normDistribution  = std::normal_distribution<float>(1.f, .1f);
        auto        angleDistribution = std::uniform_real_distribution<float>(0.f, 2 * M_PI);

        this->gridVectors.reserve(vectorsToGenerate);

        for (size_t i = 0; i < vectorsToGenerate; i++) {
            auto angle = angleDistribution(engine);
            
            auto vector = normDistribution(engine) * sf::Vector2f(cosf(angle), sinf(angle));

            this->gridVectors.push_back(vector);
        }
    }
    
    public: float
    positiveValue(float x, float y) {
        auto value = this->value(x, y);
        
        return (value + 1.f) / 2.f;
    }

    public: float
    value(float x, float y) {
        auto pointInfo = this->asPointInGrid(x, y, this->gridSubdivisions, this->squareGridSize, this->squareGridSize);

        auto snappedToGrid      = std::get<0>(pointInfo);
        auto positionWithinGrid = std::get<1>(pointInfo);
        auto cellPosition       = std::get<2>(pointInfo);

        auto topLeftValue     = this->dotProductWithCornerGradient(positionWithinGrid, snappedToGrid, sf::Vector2f(0.f, 0.f), this->squareGridSize, this->squareGridSize);
        auto topRightValue    = this->dotProductWithCornerGradient(positionWithinGrid, snappedToGrid, sf::Vector2f(1.f, 0.f), this->squareGridSize, this->squareGridSize);
        auto bottomLeftValue  = this->dotProductWithCornerGradient(positionWithinGrid, snappedToGrid, sf::Vector2f(0.f, 1.f), this->squareGridSize, this->squareGridSize);
        auto bottomRightValue = this->dotProductWithCornerGradient(positionWithinGrid, snappedToGrid, sf::Vector2f(1.f, 1.f), this->squareGridSize, this->squareGridSize);

        // This works because our cell position is between 0 and 1.
        auto topCornersValue     = smoothStep(topLeftValue,    topRightValue,    cellPosition.x);
        auto bottomCornersValue  = smoothStep(bottomLeftValue, bottomRightValue, cellPosition.x);

        return smoothStep(topCornersValue, bottomCornersValue, cellPosition.y);
    }

    private: float
    dotProductWithCornerGradient(
        const sf::Vector2f& positionWithinGrid,
        const sf::Vector2f& snappedToGrid,
        const sf::Vector2f  cornerDelta,
        unsigned short      xSize,
        unsigned short      ySize
    ) {
        auto corner = snappedToGrid + cornerDelta;

        auto offset               = corner - positionWithinGrid;
        auto randomGradientVector = this->getRandomVector(corner, xSize, ySize);

        return dotProduct(offset, randomGradientVector);
    }

    private: const sf::Vector2f
    getRandomVector(sf::Vector2f cornerPosition, unsigned short xSize, unsigned short ySize) {
        return this->gridVectors[
            wrapAroundIncrementedByOne(xSize, cornerPosition.x)
            + xSize * wrapAroundIncrementedByOne(ySize, cornerPosition.y)
        ];
    }

    using SnappedToGrid = sf::Vector2f;
    using WithinGrid    = sf::Vector2f;
    using CellPosition  = sf::Vector2f;

    using PointInfo = std::tuple<SnappedToGrid, WithinGrid, CellPosition>;

    private: PointInfo
    asPointInGrid(float x, float y, unsigned short subdivisions, unsigned short xSize, unsigned short ySize) {
        auto inGridSpace = sf::Vector2f(x / subdivisions, y / subdivisions);

        // The fractional part is between 0 and 1 because our cell size is fixed to one.
        auto fractionalPart = inGridSpace - sf::Vector2f(floorf(inGridSpace.x), floorf(inGridSpace.y));

        auto snappedToGrid = this->snapToGrid(inGridSpace, xSize, ySize);

        return { snappedToGrid, snappedToGrid + fractionalPart, fractionalPart };
    }

    private: sf::Vector2f
    snapToGrid(sf::Vector2f position, size_t xSize, size_t ySize) {
        return sf::Vector2f(static_cast<size_t>(position.x) % xSize, static_cast<size_t>(position.y) % ySize);
    }

    private: std::vector<sf::Vector2f> gridVectors;
    private: const unsigned short      squareGridSize;
    private: const unsigned short      gridSubdivisions;
};

int main() {
    sf::Image outputImage;
    unsigned short     width         = 2048;
    unsigned short     height        = 2048;
    Perlin             perlin        = Perlin(100, 128, 64);

    outputImage.create(width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            auto perlinValue = perlin.positiveValue(x, y);
            
            auto colorComponent = static_cast<uint8_t>(255.f * perlinValue);

            if (x % 128 == 0 && y % 128 == 0) {
                outputImage.setPixel(x, y, sf::Color::Red);
            } else {
                outputImage.setPixel(x, y, sf::Color(colorComponent, colorComponent, colorComponent, 255));
            }
        }
    }

    outputImage.saveToFile("output.png");

    return 0;
}

sf::Vector2f
normalize(const sf::Vector2f source) {
    if (source.x == 0.f && source.y == 0.f) {
        return source;
    }

    auto length = vectorLength(source);

    return sf::Vector2f(source.x / length, source.y / length);
}

float
vectorLength(const sf::Vector2f& source) {
    return sqrt((source.x * source.x) + (source.y * source.y));
}

float
dotProduct(const sf::Vector2f& a, const sf::Vector2f& b) {
    return a.x * b.x + a.y * b.y;
}

int
wrapAroundIncrementedByOne(int dimension, int value) {
    if (value >= dimension) {
        return value - dimension;
    }

    return value;
}

// https://stackoverflow.com/questions/42478137/perlin-noise-artifacts
float
smoothStep(float low, float high, float weight) {
    auto value = powf(weight, 3) * (6 * powf(weight, 2) - 15 * weight + 10);
    
    return low + value * (high - low);
}
