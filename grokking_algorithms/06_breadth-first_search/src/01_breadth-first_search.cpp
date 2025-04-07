//
// Created by wuxianggujun on 2025/4/7.
//
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <unordered_set>

using std::cout;
using std::endl;

// 检查名字是否以 'm' 结尾，表示是芒果销售商
bool is_seller(const std::string& name)
{
    return !name.empty() && name.back() == 'm';
}

template <typename T>
bool search(const T& name, const std::unordered_map<T, std::vector<T>>& graph)
{
    std::queue<T> search_queue;
    std::unordered_set<T> searched;

    // 关键修复1：检查起始节点是否存在
    auto start_it = graph.find(name);
    if (start_it == graph.end()) {
        cout << "Start node not found in graph!" << endl;
        return false;
    }

    // 初始化队列
    for (const auto& friend_name : start_it->second) {
        search_queue.push(friend_name);
    }

    while (!search_queue.empty()) {
        // 关键修复2：使用值拷贝而非引用
        T person = search_queue.front();
        search_queue.pop();

        if (searched.count(person)) continue;

        if (is_seller(person)) {
            cout << person << " is a mango seller." << endl;
            return true;
        }

        // 关键修复3：检查关联节点是否存在
        auto person_it = graph.find(person);
        if (person_it != graph.end()) {
            for (const auto& friend_name : person_it->second) {
                search_queue.push(friend_name);
            }
        }

        searched.insert(person);
    }

    cout << "No mango seller found." << endl;
    return false;
}


int main()
{
    std::unordered_map<std::string, std::vector<std::string>> graph;
    graph.insert({"you", {"alice", "bob", "claire"}});
    graph.insert({"bob", {"anuj", "peggy"}});
    graph.insert({"alice", {"peggy"}});
    graph.insert({"claire", {"thom", "jonny"}});
    graph.insert({"anuj", {}});
    graph.insert({"peggy", {}});
    graph.insert({"thom", {}});
    graph.insert({"jonny", {}});

    std::string name = "you";
    bool result = search(name, graph);
    cout << "Found mango seller: " << result << endl;
}
