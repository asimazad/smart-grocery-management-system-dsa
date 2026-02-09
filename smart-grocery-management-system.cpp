#include <iostream>
#include <string>
using namespace std;
//STRUCTURES
struct Grocery {
    int id;
    string name;
    string description;
    int priority;
    int deadline;
    string status;
    Grocery* next;
};
struct Stack {
    Grocery data[50];
    int top = -1;
};
struct Queue {
    Grocery data[50];
    int front = 0, rear = -1;
    bool empty() { return front > rear; }
    void enqueue(Grocery g) {
        if (rear < 49) data[++rear] = g;
    }
    void display() {
        if (empty()) {
            cout << "No completed tasks.\n";
            return;
        }
        for (int i = front; i <= rear; i++) {
            cout << data[i].id << " " << data[i].name
                << " (" << data[i].description << ") "
                << "P:" << data[i].priority
                << " D:" << data[i].deadline
                << " " << data[i].status << endl;
        }
    }
};
Grocery* head = NULL;
int idCount = 1;
Stack undoStack, redoStack;
Queue history;
Grocery lastDeleted;
bool hasLastDeleted = false;
//HEAP
struct HeapPQ {
    Grocery data[50];
    int size = 0;
    void heapifyUp(int i) {
        while (i > 0) {
            int p = (i - 1) / 2;
            if (data[i].priority <= data[p].priority) break;
            swap(data[i], data[p]);
            i = p;
        }
    }
    void heapifyDown(int i) {
        while (true) {
            int l = 2 * i + 1, r = 2 * i + 2, largest = i;
            if (l < size && data[l].priority > data[largest].priority) largest = l;
            if (r < size && data[r].priority > data[largest].priority) largest = r;
            if (largest == i) break;
            swap(data[i], data[largest]);
            i = largest;
        }
    }
    void clear() { size = 0; }
    void push(Grocery g) {
        if (size >= 50) return;
        data[size] = g;
        heapifyUp(size++);
    }
    void display() {
        for (int i = 0; i < size; i++) {
            cout << data[i].id << " " << data[i].name
                << " P:" << data[i].priority
                << " D:" << data[i].deadline
                << " " << data[i].status << endl;
        }
    }
};
HeapPQ pq;
void rebuildHeap() {
    pq.clear();
    for (Grocery* t = head; t; t = t->next)
        pq.push(*t);
}
void addItem(string name, string desc, int pr, int dl) {
    Grocery* n = new Grocery;
    n->id = idCount++;
    n->name = name;
    n->description = desc;
    n->priority = pr;
    n->deadline = dl;
    n->status = "Pending";
    n->next = head;
    head = n;
    if (undoStack.top < 49)
        undoStack.data[++undoStack.top] = *n;
    redoStack.top = -1;
    rebuildHeap();
}
void display() {
    cout << "\n-- Linked List --\n";
    for (Grocery* t = head; t; t = t->next) {
        cout << t->id << " " << t->name
            << " (" << t->description << ") "
            << "P:" << t->priority
            << " D:" << t->deadline
            << " " << t->status << endl;
    }
    cout << "\n-- Priority Queue (Heap) --\n";
    pq.display();
    cout << "\n-- Completed History (Queue) --\n";
    history.display();
}
void searchItem(string name) {
    for (Grocery* t = head; t; t = t->next) {
        if (t->name == name) {
            cout << "Found: " << t->name << endl;
            return;
        }
    }
    cout << "Item not found\n";
}
void sortByDeadline() {
    for (Grocery* i = head; i; i = i->next) {
        for (Grocery* j = i->next; j; j = j->next) {
            if (i->deadline > j->deadline) {
                swap(i->id, j->id);
                swap(i->name, j->name);
                swap(i->description, j->description);
                swap(i->priority, j->priority);
                swap(i->deadline, j->deadline);
                swap(i->status, j->status);
            }
        }
    }
}
//DELETE 
// Big-O: O(n)
void deleteItem(int id) {
    Grocery* t = head;
    Grocery* prev = NULL;
    while (t) {
        if (t->id == id) {
            lastDeleted = *t;
            hasLastDeleted = true;
            if (prev) prev->next = t->next;
            else head = t->next;
            delete t;
            rebuildHeap();
            cout << "Item deleted successfully\n";
            return;
        }
        prev = t;
        t = t->next;
    }
    cout << "Item not found\n";
}
Grocery* binarySearchByDeadline(int dl) {
    Grocery* arr[50];
    int n = 0;
    for (Grocery* t = head; t && n < 50; t = t->next)
        arr[n++] = t;
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j]->deadline < arr[min]->deadline)
                min = j;
        swap(arr[i], arr[min]);
    }
    int l = 0, r = n - 1;
    while (l <= r) {
        int m = (l + r) / 2;
        if (arr[m]->deadline == dl) return arr[m];
        else if (arr[m]->deadline < dl) l = m + 1;
        else r = m - 1;
    }
    return NULL;
}
void markCompleted(int id) {
    Grocery* t = head;
    Grocery* prev = NULL;
    while (t) {
        if (t->id == id) {
            t->status = "Purchased";
            history.enqueue(*t);
            lastDeleted = *t;
            hasLastDeleted = true;
            if (prev) prev->next = t->next;
            else head = t->next;
            delete t;
            rebuildHeap();
            cout << "Item marked completed\n";
            return;
        }
        prev = t;
        t = t->next;
    }
    cout << "Item not found\n";
}
void undoAdd() {
    if (undoStack.top < 0 || !head) return;
    if (redoStack.top < 49)
        redoStack.data[++redoStack.top] = *head;
    Grocery* temp = head;
    head = head->next;
    delete temp;
    undoStack.top--;
    rebuildHeap();
    cout << "Undo Add Successful\n";
}
void undoDelete() {
    if (!hasLastDeleted) {
        cout << "Nothing to undo\n";
        return;
    }
    Grocery* n = new Grocery(lastDeleted);
    n->next = head;
    head = n;
    rebuildHeap();
    hasLastDeleted = false;
    cout << "Undo Delete Successful\n";
}
void redoAdd() {
    if (redoStack.top < 0) return;
    Grocery g = redoStack.data[redoStack.top--];
    Grocery* n = new Grocery(g);
    n->next = head;
    head = n;
    rebuildHeap();
    cout << "Redo Successful\n";
}
void searchByPriority(int pr) {
    bool found = false;
    for (Grocery* t = head; t; t = t->next) {
        if (t->priority == pr) {
            cout << t->id << " " << t->name
                << " (" << t->description << ") "
                << "P:" << t->priority
                << " D:" << t->deadline
                << " " << t->status << endl;
            found = true;
        }
    }
    if (!found) cout << "No items found with priority " << pr << endl;
}

//MAIN
int main() {
    int choice;
    do {
        cout << "\n1.Add 2.Display 3.Search 4.Sort "
            << "5.Undo Add 6.Redo Add 7.Mark Completed "
            << "8.Binary Search 9.Delete 10.Undo Delete 11.Exit 12. search by priority\n";
        cin >> choice;
        if (choice == 1) {
            string n, d;
            int p, dl;
            cin.ignore();
            cout << "Name: "; getline(cin, n);
            cout << "Description: "; getline(cin, d);
            cout << "Priority(3-1): "; cin >> p;
            cout << "Deadline: "; cin >> dl;
            addItem(n, d, p, dl);
        }
        else if (choice == 2) display();
        else if (choice == 3) {
            string n;
            cin.ignore();
            cout << "Search: "; getline(cin, n);
            searchItem(n);
        }
        else if (choice == 4) sortByDeadline();
        else if (choice == 5) undoAdd();
        else if (choice == 6) redoAdd();
        else if (choice == 7) {
            int id; cout << "ID: "; cin >> id;
            markCompleted(id);
        }
        else if (choice == 8) {
            int dl; cout << "Deadline: "; cin >> dl;
            Grocery* g = binarySearchByDeadline(dl);
            if (g) cout << "Found: " << g->name << endl;
            else cout << "Not found\n";
        }
        else if (choice == 9) {
            int id; cout << "ID to delete: "; cin >> id;
            deleteItem(id);
        }

        else if (choice == 10) undoDelete();
        else if (choice == 12) {  // new menu option
            int pr;
            cout << "Enter priority to search: ";
            cin >> pr;
            searchByPriority(pr);
        }


    } while (choice != 11);
    return 0;
}