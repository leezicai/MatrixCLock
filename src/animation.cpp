#include "animation.h"

// Constructor
AnimationManager::AnimationManager() {
    init();
}

// Initialize the animation types vector
void AnimationManager::init() {
    // Clear any existing data and initialize with {0, 1, 2}
    animationTypes.clear();
    animationTypes = {0, 1, 2};
}

// Get next animation type based on current index
int16_t AnimationManager::nextAnimationType(int16_t index) {
    if (animationTypes.empty()) {
        return -1; // Return error value if vector is empty
    }
    return (index + 1) % static_cast<int16_t>(animationTypes.size());
}

// Get previous animation type based on current index
int16_t AnimationManager::preAnimationType(int16_t index) {
    if (animationTypes.empty()) {
        return -1; // Return error value if vector is empty
    }
    int16_t vectorSize = static_cast<int16_t>(animationTypes.size());
    return (index - 1 + vectorSize) % vectorSize;
}

// Get the size of animation types vector
size_t AnimationManager::getSize() const {
    return animationTypes.size();
}

// Global instance definition - this creates the actual object
AnimationManager animationManager;