#include <iostream>
#include <vector>
#include <map>

using namespace std;

class MyCalendarTwo {
    private:
        map<int, int> booking_counts;
        static inline constexpr int MAX_BOOKINGS = 2;

    public:
        MyCalendarTwo() {
            
        }
        
        bool book(int startTime, int endTime) {
            booking_counts[startTime]++;
            booking_counts[endTime]--;
            int bookings = 0;
            for (auto& [time, cnt] : booking_counts){
                bookings += cnt;
                if (bookings > MAX_BOOKINGS){
                    if (--booking_counts[startTime] == 0){
                        booking_counts.erase(startTime);
                    }
                    if (++booking_counts[endTime] == 0){
                        booking_counts.erase(endTime);
                    }
                    return false;
                }
            }
            return true;

        }
};

/**
 * Your MyCalendarTwo object will be instantiated and called as such:
 * MyCalendarTwo* obj = new MyCalendarTwo();
 * bool param_1 = obj->book(startTime,endTime);
 */