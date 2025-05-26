#ifndef COMPUTER_CLUB_H
#define COMPUTER_CLUB_H


#include <string>
#include <vector>
#include <map>
#include <queue>

struct Time {
    int hours, minutes;
    Time(const std::string& time_str);
    Time(int h = 0, int m = 0);
    bool operator<(const Time& other) const;
    bool operator>=(const Time& other) const;
    std::string to_string() const;
    int diff_minutes(const Time& other) const;
};

struct Table {
    int revenue = 0;
    int occupied_time = 0;
    std::string client;
    Time start_time;
    bool is_occupied = false;
};

struct Event {
    Time time;
    int id;
    std::string client;
    int table = 0;
    Event(const std::string& line);
};

class ComputerClub {
private:
    int num_tables;
    Time start_time, end_time;
    int cost_per_hour;
    std::vector<Table> tables;
    std::map<std::string, Time> clients;
    std::queue<std::string> waiting_queue;
    std::vector<std::string> output;

    void process_client_arrived(const Event& event);
    void process_client_sat(const Event& event);
    void process_client_waiting(const Event& event);
    void process_client_left(const Event& event);
    void close_club();

public:
    ComputerClub(const std::string& filename);
};

#endif // COMPUTER_CLUB_H