#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <unordered_map>
#include <string>
#include <iomanip>
#include <limits>

/**
 * @brief Represents a memory segment (for segmentation simulation)
 */
struct Segment {
    std::string name;
    size_t base;
    size_t limit;
    Segment(const std::string& n, size_t b, size_t l) : name(n), base(b), limit(l) {}
};

/**
 * @brief Represents a page table entry
 */
struct PageTableEntry {
    int frameNumber; ///< Frame number if page is loaded
    bool valid;      ///< Valid bit
    PageTableEntry() : frameNumber(-1), valid(false) {}
};

/**
 * @brief Page replacement policy
 */
enum class ReplacementPolicy {
    FIFO,
    LRU
};

/**
 * @brief Simulates a Virtual Memory Manager with paging, segmentation, and page replacement
 */
class VirtualMemoryManager {
    size_t pageSize;
    size_t numFrames;
    size_t numPages;
    std::vector<Segment> segments;
    std::vector<PageTableEntry> pageTable;
    std::vector<int> frameTable; ///< frameTable[frame] = page number or -1
    ReplacementPolicy policy;
    // FIFO
    std::queue<size_t> fifoQueue;
    // LRU
    std::list<size_t> lruList; // most recently used at front
    std::unordered_map<size_t, std::list<size_t>::iterator> lruMap; // pageNum -> iterator in lruList
    size_t pageFaults;
    size_t accesses;

public:
    /**
     * @brief Constructor
     * @param memSize Total memory size
     * @param pageSz Page size
     * @param segNames Names of segments
     * @param pol Page replacement policy
     */
    explicit VirtualMemoryManager(size_t memSize, size_t pageSz, const std::vector<std::string>& segNames, ReplacementPolicy pol)
        : pageSize(pageSz), pageFaults(0), accesses(0), policy(pol) {
        numFrames = memSize / pageSize;
        numPages = memSize / pageSize;
        pageTable.resize(numPages);
        frameTable.assign(numFrames, -1);
        // Create segments
        size_t nSegments = segNames.size();
        size_t segSize = memSize / nSegments;
        for (size_t i = 0; i < nSegments; ++i) {
            segments.emplace_back(segNames[i], i * segSize, segSize);
        }
    }

    /**
     * @brief Display all segments
     */
    void showSegments() const {
        std::cout << "\nSegments:\n";
        for (size_t i = 0; i < segments.size(); ++i) {
            const auto& seg = segments[i];
            std::cout << i << ": " << seg.name << ": Base = " << seg.base << ", Limit = " << seg.limit << '\n';
        }
    }

    /**
     * @brief Display the page table
     */
    void showPageTable() const {
        std::cout << "\nPage Table (Page -> Frame):\n";
        for (size_t i = 0; i < pageTable.size(); ++i) {
            if (pageTable[i].valid)
                std::cout << "Page " << i << " -> Frame " << pageTable[i].frameNumber << '\n';
            else
                std::cout << "Page " << i << " -> Not in memory\n";
        }
    }

    /**
     * @brief Display the frame table
     */
    void showFrames() const {
        std::cout << "\nFrames (Frame -> Page):\n";
        for (size_t i = 0; i < frameTable.size(); ++i) {
            if (frameTable[i] != -1)
                std::cout << "Frame " << i << " -> Page " << frameTable[i] << '\n';
            else
                std::cout << "Frame " << i << " -> Empty\n";
        }
    }

    /**
     * @brief Access a logical address (segment + offset)
     * @param segIdx Segment index
     * @param offset Offset within segment
     */
    void accessAddress(size_t segIdx, size_t offset) {
        if (segIdx >= segments.size()) {
            std::cout << "Invalid segment index!\n";
            return;
        }
        const Segment& seg = segments[segIdx];
        if (offset >= seg.limit) {
            std::cout << "Offset out of bounds!\n";
            return;
        }
        size_t logicalAddr = seg.base + offset;
        size_t pageNum = logicalAddr / pageSize;
        size_t pageOffset = logicalAddr % pageSize;
        ++accesses;
        if (!pageTable[pageNum].valid) {
            ++pageFaults;
            handlePageFault(pageNum);
            std::cout << "Page fault occurred! Loaded page " << pageNum << " into memory.\n";
        }
        if (policy == ReplacementPolicy::LRU) updateLRU(pageNum);
        int frameNum = pageTable[pageNum].frameNumber;
        size_t physicalAddr = static_cast<size_t>(frameNum) * pageSize + pageOffset;
        std::cout << "Logical Address: " << logicalAddr << " (Segment " << segIdx << ", Offset " << offset << ")\n";
        std::cout << "Physical Address: " << physicalAddr << " (Frame " << frameNum << ", Offset " << pageOffset << ")\n";
    }

    /**
     * @brief Handle a page fault using selected replacement policy
     * @param pageNum The page number to load
     */
    void handlePageFault(size_t pageNum) {
        int freeFrame = -1;
        for (size_t i = 0; i < frameTable.size(); ++i) {
            if (frameTable[i] == -1) {
                freeFrame = static_cast<int>(i);
                break;
            }
        }
        if (freeFrame == -1) {
            if (policy == ReplacementPolicy::FIFO) {
                size_t victimPage = fifoQueue.front(); fifoQueue.pop();
                freeFrame = pageTable[victimPage].frameNumber;
                pageTable[victimPage].valid = false;
                frameTable[freeFrame] = -1;
            } else if (policy == ReplacementPolicy::LRU) {
                size_t victimPage = lruList.back();
                lruList.pop_back();
                lruMap.erase(victimPage);
                freeFrame = pageTable[victimPage].frameNumber;
                pageTable[victimPage].valid = false;
                frameTable[freeFrame] = -1;
            }
        }
        // Load page into frame
        pageTable[pageNum].frameNumber = freeFrame;
        pageTable[pageNum].valid = true;
        frameTable[freeFrame] = static_cast<int>(pageNum);
        if (policy == ReplacementPolicy::FIFO) fifoQueue.push(pageNum);
        else if (policy == ReplacementPolicy::LRU) addLRU(pageNum);
    }

    /**
     * @brief Add a page to the LRU list
     */
    void addLRU(size_t pageNum) {
        lruList.push_front(pageNum);
        lruMap[pageNum] = lruList.begin();
    }

    /**
     * @brief Update LRU list on page access
     */
    void updateLRU(size_t pageNum) {
        auto it = lruMap.find(pageNum);
        if (it != lruMap.end()) {
            lruList.erase(it->second);
            lruList.push_front(pageNum);
            lruMap[pageNum] = lruList.begin();
        }
    }

    /**
     * @brief Show statistics (accesses, page faults, fault rate)
     */
    void showStats() const {
        std::cout << "\nStatistics:\n";
        std::cout << "Total accesses: " << accesses << '\n';
        std::cout << "Page faults: " << pageFaults << '\n';
        if (accesses > 0)
            std::cout << "Page fault rate: " << std::fixed << std::setprecision(2) << (100.0 * pageFaults / accesses) << "%\n";
    }

    size_t getNumSegments() const { return segments.size(); }
    size_t getSegmentLimit(size_t segIdx) const { return segments[segIdx].limit; }
    std::string getSegmentName(size_t segIdx) const { return segments[segIdx].name; }
};

/**
 * @brief Menu options for the CLI
 */
enum MenuOption {
    SHOW_SEGMENTS = 1,
    SHOW_PAGETABLE = 2,
    SHOW_FRAMES = 3,
    ACCESS_ADDRESS = 4,
    SHOW_STATS = 5,
    EXIT = 0
};

void menu() {
    std::cout << "\nVirtual Memory Manager Simulator\n";
    std::cout << "1. Show Segments\n";
    std::cout << "2. Show Page Table\n";
    std::cout << "3. Show Frames\n";
    std::cout << "4. Access Address\n";
    std::cout << "5. Show Statistics\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter choice: ";
}

int main() {
    size_t memSize, pageSize, nSegments;
    std::cout << "Enter total memory size (bytes): ";
    std::cin >> memSize;
    std::cout << "Enter page size (bytes): ";
    std::cin >> pageSize;
    std::cout << "Enter number of segments: ";
    std::cin >> nSegments;
    std::vector<std::string> segNames;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (size_t i = 0; i < nSegments; ++i) {
        std::string name;
        std::cout << "Enter name for segment " << i << ": ";
        std::getline(std::cin, name);
        if (name.empty()) name = "Segment" + std::to_string(i);
        segNames.push_back(name);
    }
    int polChoice = 0;
    std::cout << "Select page replacement policy (1 = FIFO, 2 = LRU): ";
    std::cin >> polChoice;
    ReplacementPolicy policy = (polChoice == 2) ? ReplacementPolicy::LRU : ReplacementPolicy::FIFO;
    VirtualMemoryManager vmm(memSize, pageSize, segNames, policy);
    int choice = -1;
    while (true) {
        menu();
        std::cin >> choice;
        if (!std::cin) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input!\n";
            continue;
        }
        switch (choice) {
            case SHOW_SEGMENTS:
                vmm.showSegments();
                break;
            case SHOW_PAGETABLE:
                vmm.showPageTable();
                break;
            case SHOW_FRAMES:
                vmm.showFrames();
                break;
            case ACCESS_ADDRESS: {
                size_t segIdx, offset;
                vmm.showSegments();
                std::cout << "Enter segment index (0-" << vmm.getNumSegments() - 1 << "): ";
                std::cin >> segIdx;
                if (!std::cin || segIdx >= vmm.getNumSegments()) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid segment index!\n";
                    break;
                }
                std::cout << "Enter offset (0-" << vmm.getSegmentLimit(segIdx) - 1 << "): ";
                std::cin >> offset;
                if (!std::cin || offset >= vmm.getSegmentLimit(segIdx)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid offset!\n";
                    break;
                }
                vmm.accessAddress(segIdx, offset);
                break;
            }
            case SHOW_STATS:
                vmm.showStats();
                break;
            case EXIT:
                std::cout << "Exiting...\n";
                return 0;
            default:
                std::cout << "Invalid choice!\n";
        }
    }
    return 0;
} 