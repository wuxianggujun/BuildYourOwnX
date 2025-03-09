//
// Created by wuxianggujun on 2025/1/19.
// https://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

// 定义栈的最大容量为 256
constexpr int STACK_MAX = 256;
// 定义初始对象数量上限为 8
constexpr int INIT_OBJ_NUM_MAX = 8;

// 定义对象类型枚举：整数 (INT) 和 pair 对 (PAIR)
enum class ObjectType {
    INT,
    PAIR
};

// 定义对象类
class Object {
public:
    // 对象类型
    ObjectType type;
    // 标记位，用于垃圾回收中的标记阶段
    bool marked;

    // 指向链表中下一个对象的智能指针
    std::shared_ptr<Object> next;

    // 内部类，用于存储对象的值，类似于 C 语言中的 union
    class Value {
    public:
        // 整数值，用于 OBJ_INT 类型的对象
        int intValue;

        // 结构体，用于 OBJ_PAIR 类型的对象，包含两个指向其他对象的智能指针
        struct {
            std::shared_ptr<Object> head;
            std::shared_ptr<Object> tail;
        } pairValue;

        // 构造函数，初始化 intValue 为 0
        Value() : intValue(0) {
        }
    };

    // 对象的值
    Value value;

    // 构造函数，初始化对象类型和标记位
    explicit Object(const ObjectType type) : type(type), marked(false) {
    }
};

// 定义虚拟机类
class VM {
public:
    // 对象栈，用于存储对象
    std::vector<std::shared_ptr<Object>> stack;
    // 栈的大小
    int stackSize;

    // 指向堆中第一个对象的智能指针
    std::shared_ptr<Object> firstObject;
    // 当前对象的数量
    int numObjects;
    // 触发垃圾回收的对象数量上限
    int maxObjects;

    // 构造函数，初始化栈大小、第一个对象指针、对象数量和对象数量上限
    VM() : stackSize(0), firstObject(nullptr), numObjects(0), maxObjects(INIT_OBJ_NUM_MAX) {
        // 预分配栈的容量，避免多次重新分配内存
        stack.reserve(STACK_MAX);
    }

    // 将对象压入栈中
    void push(const std::shared_ptr<Object>& value) {
        // 检查栈是否溢出
        if (stackSize >= STACK_MAX) {
            std::cerr << "Stack overflow!" << std::endl;
            exit(1);
        }
        // 将对象添加到栈顶
        stack.push_back(value);
        // 栈大小加 1
        stackSize++;
    }

    // 将对象从栈中弹出
    std::shared_ptr<Object> pop() {
        // 检查栈是否下溢
        if (stackSize <= 0) {
            std::cerr << "Stack underflow!" << std::endl;
            exit(1);
        }
        // 获取栈顶对象
        std::shared_ptr<Object> obj = stack.back();
        // 将栈顶对象移除
        stack.pop_back();
        // 栈大小减 1
        stackSize--;
        // 返回弹出的对象
        return obj;
    }

    // 标记对象，用于垃圾回收的标记阶段
    void mark(const std::shared_ptr<Object>& object) {
        // 如果对象已被标记，则直接返回
        if (object->marked) return;
        // 标记对象
        object->marked = true;
        // 如果对象类型为 PAIR，则递归标记其 head 和 tail
        if (object->type == ObjectType::PAIR) {
            mark(object->value.pairValue.head);
            mark(object->value.pairValue.tail);
        }
    }

    // 标记所有可达对象
    void markAll() {
        // 遍历栈中的所有对象，并进行标记
        for (int i = 0; i < stackSize; i++) {
            mark(stack[i]);
        }
    }

    // 清除未标记的对象，用于垃圾回收的清除阶段
    void sweep() {
        // 使用指针的指针来遍历对象链表，方便删除节点
        std::shared_ptr<Object>* object = &firstObject;
        while (*object) {
            // 如果对象未被标记，则将其从链表中移除
            if (!(*object)->marked) {
                std::shared_ptr<Object> unreached = *object;
                *object = unreached->next;
                // 智能指针会自动释放内存
                numObjects--;
            } else {
                // 如果对象被标记，则取消标记，为下一次垃圾回收做准备
                (*object)->marked = false;
                object = &(*object)->next;
            }
        }
    }

    // 垃圾回收函数
    void gc() {
        // 记录垃圾回收前的对象数量
        const int numObjectsBefore = numObjects;

        // 标记所有可达对象
        markAll();
        // 清除未标记的对象
        sweep();

        // 更新对象数量上限，如果当前对象数量为 0，则重置为初始值，否则设置为当前对象数量的两倍
        maxObjects = numObjects == 0 ? INIT_OBJ_NUM_MAX : numObjects * 2;

        // 输出垃圾回收信息
        std::cout << "Collected " << numObjectsBefore - numObjects << " objects, "
                  << numObjects << " remaining." << std::endl;
    }

    // 创建新对象
    std::shared_ptr<Object> newObject(ObjectType type) {
        // 如果当前对象数量达到上限，则触发垃圾回收
        if (numObjects == maxObjects) gc();

        // 创建新对象
        auto object = std::make_shared<Object>(type);
        // 将新对象添加到链表头部
        object->next = firstObject;
        firstObject = object;
        // 将新对象的标记位设置为 false
        object->marked = false;

        // 对象数量加 1
        numObjects++;

        // 返回新对象
        return object;
    }

    // 创建整数对象并将其压入栈中
    void pushInt(int intValue) {
        // 创建整数对象
        std::shared_ptr<Object> object = newObject(ObjectType::INT);
        // 设置对象的值
        object->value.intValue = intValue;
        // 将对象压入栈中
        push(object);
    }

    // 创建 pair 对象并将其压入栈中
    std::shared_ptr<Object> pushPair() {
        // 创建 pair 对象
        std::shared_ptr<Object> object = newObject(ObjectType::PAIR);
        // 弹出栈顶的两个对象，分别作为 pair 对象的 tail 和 head
        object->value.pairValue.tail = pop();
        object->value.pairValue.head = pop();
        // 将 pair 对象压入栈中
        push(object);
        // 返回 pair 对象
        return object;
    }

    // 打印对象
    void objectPrint(const std::shared_ptr<Object>& object) {
        // 根据对象类型进行不同的打印操作
        switch (object->type) {
        case ObjectType::INT:
            // 打印整数值
            std::cout << object->value.intValue;
            break;
        case ObjectType::PAIR:
            // 打印 pair 对象，递归打印 head 和 tail
            std::cout << "(";
            objectPrint(object->value.pairValue.head);
            std::cout << ", ";
            objectPrint(object->value.pairValue.tail);
            std::cout << ")";
            break;
        }
    }
};

// 测试函数 1：测试栈上的对象是否被正确保留
void test1() {
    std::cout << "Test 1: Objects on stack are preserved." << std::endl;
    // 创建虚拟机实例
    VM vm;
    // 压入两个整数对象
    vm.pushInt(1);
    vm.pushInt(2);

    // 执行垃圾回收
    vm.gc();
    // 检查对象数量是否为 2
    if (vm.numObjects != 2) {
        std::cerr << "Should have preserved objects." << std::endl;
        exit(1);
    }
}

// 测试函数 2：测试未使用的对象是否被正确回收
void test2() {
    std::cout << "Test 2: Unreached objects are collected." << std::endl;
    // 创建虚拟机实例
    VM vm;
    // 压入两个整数对象
    vm.pushInt(1);
    vm.pushInt(2);
    // 弹出两个对象
    vm.pop();
    vm.pop();

    // 执行垃圾回收
    vm.gc();
    // 检查对象数量是否为 0
    if (vm.numObjects != 0) {
        std::cerr << "Should have collected objects." << std::endl;
        exit(1);
    }
}

// 测试函数 3：测试嵌套对象的可达性
void test3() {
    std::cout << "Test 3: Reach nested objects." << std::endl;
    // 创建虚拟机实例
    VM vm;
    // 压入整数和 pair 对象
    vm.pushInt(1);
    vm.pushInt(2);
    vm.pushPair();
    vm.pushInt(3);
    vm.pushInt(4);
    vm.pushPair();
    vm.pushPair();

    // 执行垃圾回收
    vm.gc();
    // 检查对象数量是否为 7
    if (vm.numObjects != 7) {
        std::cerr << "Should have reached objects." << std::endl;
        exit(1);
    }
}

// 测试函数 4：测试循环引用的处理
void test4() {
    std::cout << "Test 4: Handle cycles." << std::endl;
    // 创建虚拟机实例
    VM vm;
    // 压入整数和 pair 对象
    vm.pushInt(1);
    vm.pushInt(2);
    std::shared_ptr<Object> a = vm.pushPair();
    vm.pushInt(3);
    vm.pushInt(4);
    std::shared_ptr<Object> b = vm.pushPair();

    // 设置循环引用：a 的 tail 指向 b，b 的 tail 指向 a
    a->value.pairValue.tail = b;
    b->value.pairValue.tail = a;

    // 执行垃圾回收
    vm.gc();
    // 检查对象数量是否为 4 (四个直接可达的对象：1, 2, a, b，其中 a 和 b 构成循环引用)
    if (vm.numObjects != 4) {
        std::cerr << "Should have collected objects." << std::endl;
        exit(1);
    }
}

// 性能测试函数
void perfTest() {
    std::cout << "Performance Test." << std::endl;
    // 创建虚拟机实例
    VM vm;

    // 执行 1000 次循环
    for (int i = 0; i < 1000; i++) {
        // 每次循环压入 20 个整数对象
        for (int j = 0; j < 20; j++) {
            vm.pushInt(i);
        }
        // 每次循环弹出 20 个对象
        for (int k = 0; k < 20; k++) {
            vm.pop();
        }
    }
}

// 主函数
int main(int argc, const char* argv[]) {
    // 执行测试函数
    test1();
    test2();
    test3();
    test4();
    //perfTest(); // 可以选择是否执行性能测试

    return 0;
}