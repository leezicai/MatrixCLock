#ifndef ANIMATION_H
#define ANIMATION_H
#include <vector>
#include <cstdint>

class AnimationManager {
private:
    std::vector<int16_t> animationTypes;
public:
    // Constructor
    AnimationManager();
    
    // Initialize the animation types vector
    void init();
    
    // Get next animation type based on current index
    int16_t nextAnimationType(int16_t index);
    
    // Get previous animation type based on current index
    int16_t preAnimationType(int16_t index);
    
    // Get the size of animation types vector
    size_t getSize() const;
};

// Global instance declaration for external use
extern AnimationManager animationManager;

#endif // ANIMATION_H