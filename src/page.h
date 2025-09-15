// page.h
#pragma once
#include <cstdlib>

// Page definitions
#define PAGE_0 0
#define PAGE_1 1
#define MAX_FIRST_CLASS_PAGE 2

// Setting page (for PAGE_ZERO)
#define PAGE_0_0 0
#define PAGE_0_1 1
#define PAGE_0_2 2
#define PAGE_0_3 3
#define PAGE_0_4 4
#define PAGE_0_5 5
#define MAX_SECOND_CLASS_0_PAGE 6

// Page 1 sub-pages
#define PAGE_1_0 0
#define PAGE_1_1 1
#define PAGE_1_2 2
#define MAX_SECOND_CLASS_1_PAGE 3

#define ANIMATION_TYPE_0 0
#define ANIMATION_TYPE_1 1
#define ANIMATION_TYPE_2 2
#define MAX_ANIMATION_TYPE 3
class Page {
private:
    int currentFirstClassPage;
    int currentSecondClassPage;
    int history_page_zero_second_class_page;
    int history_page_one_second_class_page;
    int currentAnimationType;

    // Get max second class page for current first class page
    int getMaxSecondClassPage() const;
    
    // Save current second class page to history
    void saveCurrentSecondClassToHistory();
    
    // Load second class page from history for given first class page
    void loadSecondClassFromHistory(int firstClassPage);

public:
    // Constructor
    Page();
    
    // Destructor
    ~Page();
    
    // Increase first class page (with circular logic)
    void increaseFirstClassPage();
    
    // Decrease first class page (with circular logic)
    void decreaseFirstClassPage();
    
    // Increase second class page (with circular logic)
    void increaseSecondClassPage();
    
    // Decrease second class page (with circular logic)
    void decreaseSecondClassPage();

     // Increase first class page (with circular logic)
    void increaseAnimationType();
    
    // Decrease first class page (with circular logic)
    void decreaseAnimationType();

    void setAnimationType(int type);

    // Getters
    int getCurrentFirstClassPage() const { return currentFirstClassPage; }
    int getCurrentSecondClassPage() const { return currentSecondClassPage; }
    int getCurrentAnimationType() const { return currentAnimationType; }
    int getHistoryPageZeroSecondClassPage() const { return history_page_zero_second_class_page; }
    int getHistoryPageOneSecondClassPage() const { return history_page_one_second_class_page; }
};

// Global page object declaration
extern Page page;