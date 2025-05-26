#include "ComputerClub.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>

Time::Time(const std::string& time_str) {
    hours = std::stoi(time_str.substr(0, 2));
    minutes = std::stoi(time_str.substr(3, 2));
}

Time::Time(int h, int m) : hours(h), minutes(m) {}

bool Time::operator<(const Time& other) const {
    return hours * 60 + minutes < other.hours * 60 + other.minutes;
}

bool Time::operator>=(const Time& other) const {
    return !(*this < other);
}

std::string Time::to_string() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":" 
        << std::setfill('0') << std::setw(2) << minutes;
    return oss.str();
}

int Time::diff_minutes(const Time& other) const {
    return (hours - other.hours) * 60 + (minutes - other.minutes);
}

Event::Event(const std::string& line) {
    std::istringstream iss(line);
    std::string time_str;
    iss >> time_str >> id >> client;
    time = Time(time_str);
    if (id == 2) iss >> table;
}

void ComputerClub::process_client_arrived(const Event& event) {
    output.push_back(event.time.to_string() + " 1 " + event.client);
    if (event.time < start_time || event.time >= end_time) {
        output.push_back(event.time.to_string() + " 13 NotOpenYet");
        return;
    }
    if (clients.find(event.client) != clients.end()) {
        output.push_back(event.time.to_string() + " 13 YouShallNotPass");
        return;
    }
    clients[event.client] = event.time;
}

void ComputerClub::process_client_sat(const Event& event) {
    output.push_back(event.time.to_string() + " 2 " + event.client + " " + std::to_string(event.table));
    if (clients.find(event.client) == clients.end()) {
        output.push_back(event.time.to_string() + " 13 ClientUnknown");
        return;
    }
    if (tables[event.table - 1].is_occupied) {
        output.push_back(event.time.to_string() + " 13 PlaceIsBusy");
        return;
    }
    if (!tables[event.table - 1].client.empty()) {
        clients.erase(tables[event.table - 1].client);
    }
    tables[event.table - 1].is_occupied = true;
    tables[event.table - 1].client = event.client;
    tables[event.table - 1].start_time = event.time;
    clients.erase(event.client);
}

void ComputerClub::process_client_waiting(const Event& event) {
    output.push_back(event.time.to_string() + " 3 " + event.client);
    if (clients.find(event.client) == clients.end()) {
        output.push_back(event.time.to_string() + " 13 ClientUnknown");
        return;
    }
    bool has_free_table = false;
    for (const auto& table : tables) {
        if (!table.is_occupied) {
            has_free_table = true;
            break;
        }
    }
    if (has_free_table) {
        output.push_back(event.time.to_string() + " 13 ICanWaitNoLonger!");
        return;
    }
    if (waiting_queue.size() >= num_tables) {
        output.push_back(event.time.to_string() + " 11 " + event.client);
        clients.erase(event.client);
        return;
    }
    waiting_queue.push(event.client);
    clients.erase(event.client);
}

void ComputerClub::process_client_left(const Event& event) {
    output.push_back(event.time.to_string() + " 4 " + event.client);
    if (clients.find(event.client) == clients.end()) {
        bool found = false;
        for (int i = 0; i < num_tables; ++i) {
            if (tables[i].client == event.client) {
                found = true;
                int minutes = event.time.diff_minutes(tables[i].start_time);
                tables[i].occupied_time += minutes;
                tables[i].revenue += (minutes + 59) / 60 * cost_per_hour;
                tables[i].is_occupied = false;
                tables[i].client.clear();
                if (!waiting_queue.empty()) {
                    std::string next_client = waiting_queue.front();
                    waiting_queue.pop();
                    tables[i].is_occupied = true;
                    tables[i].client = next_client;
                    tables[i].start_time = event.time;
                    output.push_back(event.time.to_string() + " 12 " + next_client + " " + std::to_string(i + 1));
                }
                break;
            }
        }
        if (!found) {
            output.push_back(event.time.to_string() + " 13 ClientUnknown");
        }
    } else {
        output.push_back(event.time.to_string() + " 13 ClientUnknown");
    }
}

void ComputerClub::close_club() {
    std::vector<std::string> remaining_clients;
    for (int i = 0; i < num_tables; ++i) {
        if (tables[i].is_occupied) {
            remaining_clients.push_back(tables[i].client);
            int minutes = end_time.diff_minutes(tables[i].start_time);
            tables[i].occupied_time += minutes;
            tables[i].revenue += (minutes + 59) / 60 * cost_per_hour;
        }
    }
    std::sort(remaining_clients.begin(), remaining_clients.end());
    for (const auto& client : remaining_clients) {
        output.push_back(end_time.to_string() + " 11 " + client);
    }
}

ComputerClub::ComputerClub(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error opening file" << std::endl;
        return;
    }

    std::string line;
    if (!std::getline(file, line)) {
        std::cout << line << std::endl;
        return;
    }
    num_tables = std::stoi(line);
    tables.resize(num_tables);

    if (!std::getline(file, line)) {
        std::cout << line << std::endl;
        return;
    }
    std::istringstream iss(line);
    std::string start, end;
    iss >> start >> end;
    start_time = Time(start);
    end_time = Time(end);

    if (!std::getline(file, line)) {
        std::cout << line << std::endl;
        return;
    }
    cost_per_hour = std::stoi(line);

    output.push_back(start_time.to_string());
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        Event event(line);
        switch (event.id) {
            case 1: process_client_arrived(event); break;
            case 2: process_client_sat(event); break;
            case 3: process_client_waiting(event); break;
            case 4: process_client_left(event); break;
            default: std::cout << line << std::endl; return;
        }
    }
    close_club();
    output.push_back(end_time.to_string());

    for (int i = 0; i < num_tables; ++i) {
        int hours = tables[i].occupied_time / 60;
        int minutes = tables[i].occupied_time % 60;
        std::ostringstream oss;
        oss << i + 1 << " " << tables[i].revenue << " " 
            << std::setfill('0') << std::setw(2) << hours << ":" 
            << std::setfill('0') << std::setw(2) << minutes;
        output.push_back(oss.str());
    }

    for (const auto& out : output) {
        std::cout << out << std::endl;
    }
}