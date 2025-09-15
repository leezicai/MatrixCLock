// page.cpp
#include "page.h"

// Global page object definition
Page page;

// Constructor
Page::Page() 
    : currentFirstClassPage(0),
      currentSecondClassPage(0),
      history_page_zero_second_class_page(0),
      history_page_one_second_class_page(0) {
}

// Destructor
Page::~Page() {
}

// Get max second class page for current first class page
int Page::getMaxSecondClassPage() const {
    switch (currentFirstClassPage) {
        case PAGE_0:
            return MAX_SECOND_CLASS_0_PAGE;
        case PAGE_1:
            return MAX_SECOND_CLASS_1_PAGE;
        default:
            return 1;  // Default to at least 1 page
    }
}

// Save current second class page to history
void Page::saveCurrentSecondClassToHistory() {
    if (currentFirstClassPage == PAGE_0) {
        history_page_zero_second_class_page = currentSecondClassPage;
    } else if (currentFirstClassPage == PAGE_1) {
        history_page_one_second_class_page = currentSecondClassPage;
    }
}

// Load second class page from history for given first class page
void Page::loadSecondClassFromHistory(int firstClassPage) {
    if (firstClassPage == PAGE_0) {
        currentSecondClassPage = history_page_zero_second_class_page;
    } else if (firstClassPage == PAGE_1) {
        currentSecondClassPage = history_page_one_second_class_page;
    } else {
        currentSecondClassPage = 0;
    }
}

// Increase first class page (with circular logic)
void Page::increaseFirstClassPage() {
    // Save current second class page to history before switching
    saveCurrentSecondClassToHistory();
    
    // Update first class page with circular increment
    currentFirstClassPage = (currentFirstClassPage + 1) % MAX_FIRST_CLASS_PAGE;
    
    // Load the second class page from history for the new first class page
    loadSecondClassFromHistory(currentFirstClassPage);
}

// Decrease first class page (with circular logic)
void Page::decreaseFirstClassPage() {
    // Save current second class page to history before switching
    saveCurrentSecondClassToHistory();
    
    // Update first class page with circular decrement
    // Add MAX_FIRST_CLASS_PAGE before modulo to handle negative numbers correctly
    currentFirstClassPage = (currentFirstClassPage - 1 + MAX_FIRST_CLASS_PAGE) % MAX_FIRST_CLASS_PAGE;
    
    // Load the second class page from history for the new first class page
    loadSecondClassFromHistory(currentFirstClassPage);
}

// Increase second class page (with circular logic)
void Page::increaseSecondClassPage() {
    int maxSecondClassPage = getMaxSecondClassPage();
    
    // Update second class page with circular increment
    currentSecondClassPage = (currentSecondClassPage + 1) % maxSecondClassPage;
    
    // Update history for current first class page
    saveCurrentSecondClassToHistory();
}

// Decrease second class page (with circular logic)
void Page::decreaseSecondClassPage() {
    int maxSecondClassPage = getMaxSecondClassPage();
    
    // Update second class page with circular decrement
    // Add maxSecondClassPage before modulo to handle negative numbers correctly
    currentSecondClassPage = (currentSecondClassPage - 1 + maxSecondClassPage) % maxSecondClassPage;
    
    // Update history for current first class page
    saveCurrentSecondClassToHistory();
}


// Increase first class page (with circular logic)
void Page::increaseAnimationType() {
    currentAnimationType = (currentAnimationType + 1) % MAX_ANIMATION_TYPE;
}

// Decrease first class page (with circular logic)
void Page::decreaseAnimationType() {
    
    currentAnimationType = (currentAnimationType - 1 + MAX_ANIMATION_TYPE) % MAX_ANIMATION_TYPE;
}

void Page::setAnimationType(int type) {
  type = abs(type);
  currentAnimationType = type;
}