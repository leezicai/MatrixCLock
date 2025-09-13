// page.h
#pragma once

// Page definitions
#define PAGE_ZERO 0
#define PAGE_ONE 1
#define MAX_FIRST_CLASS_PAGE 2

// Setting page (for PAGE_ZERO)
#define PAGE_ZERO_ZERO 0
#define MAX_SECOND_CLASS_ZERO_PAGE 1

// Page 1 sub-pages
#define PAGE_ONE_ZERO 0
#define PAGE_ONE_ONE 1
#define PAGE_ONE_TWO 2
#define MAX_SECOND_CLASS_ONE_PAGE 3

class Page {
private:
    int currentFirstClassPage;
    int currentSecondClassPage;
    int history_page_zero_second_class_page;
    int history_page_one_second_class_page;
    
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
    
    // Getters
    int getCurrentFirstClassPage() const { return currentFirstClassPage; }
    int getCurrentSecondClassPage() const { return currentSecondClassPage; }
    int getHistoryPageZeroSecondClassPage() const { return history_page_zero_second_class_page; }
    int getHistoryPageOneSecondClassPage() const { return history_page_one_second_class_page; }
};

// Global page object declaration
extern Page page;