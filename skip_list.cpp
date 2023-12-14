#include <iostream>
#include <random>
using namespace std;

template <typename T, typename Compare = less<T>>
class SkipList
{
private:
    struct Node
    {
        T key;
        vector<Node *> point;

        Node(const T &k, int lvl) : key(k), point(lvl + 1) {}
    };

    Node *head;
    Compare comp;
    int maxlvl;
    float probability;
    default_random_engine randEngine;
    uniform_real_distribution<float> randDist;

    int randomlvl()
    {
        int lvl = 0;
        while (randDist(randEngine) < probability && lvl < maxlvl)
        {
            lvl++;
        }
        return lvl;
    }

public:
    SkipList(int maxlvls = 16, float prob = 0.5)
        : maxlvl(maxlvls), probability(prob), randEngine(random_device()()), randDist(0.0, 1.0)
    {
        head = new Node(T(), maxlvl);
    }

    ~SkipList()
    {
        Node *current = head;
        while (current)
        {
            Node *temp = current;
            current = current->point[0];
            delete temp;
        }
    }

    void insert(const T &key)
    {
        Node *update[maxlvl + 1];
        Node *current = head;

        for (int i = maxlvl; i >= 0; --i)
        {
            while (current->point[i] && comp(current->point[i]->key, key))
            {
                current = current->point[i];
            }
            update[i] = current;
        }

        current = current->point[0];

        if (!current || !comp(current->key, key))
        {
            int newlvl = randomlvl();
            if (newlvl > maxlvl)
            {
                for (int i = maxlvl + 1; i <= newlvl; ++i)
                {
                    update[i] = head;
                }
                maxlvl = newlvl;
            }

            Node *newNode = new Node(key, newlvl);

            for (int i = 0; i <= newlvl; ++i)
            {
                newNode->point[i] = update[i]->point[i];
                update[i]->point[i] = newNode;
            }
        }
    }

    void remove(const T &key)
    {
        Node *update[maxlvl + 1];
        Node *current = head;

        for (int i = maxlvl; i >= 0; --i)
        {
            while (current->point[i] && comp(current->point[i]->key, key))
            {
                current = current->point[i];
            }
            update[i] = current;
        }

        current = current->point[0];

        while (current && !comp(key, current->key) && !comp(current->key, key))
        {
            for (int i = 0; i <= maxlvl; ++i)
            {
                if (update[i]->point[i] != current)
                {
                    break;
                }
                update[i]->point[i] = current->point[i];
            }
            Node *temp = current;
            current = current->point[0];
            delete temp;
        }

        while (maxlvl > 0 && !head->point[maxlvl])
        {
            maxlvl--;
        }
    }

    bool search(const T &key)
    {
        Node *current = head;
        for (int i = maxlvl; i >= 0; --i)
        {
            while (current->point[i] && comp(current->point[i]->key, key))
            {
                current = current->point[i];
            }
        }
        current = current->point[0];

        return current && !comp(key, current->key) && !comp(current->key, key);
    }

    int count_occurrence(const T &key)
    {
        Node *current = head->point[0];
        int count = 0;

        while (current && !comp(key, current->key))
        {
            if (!comp(current->key, key))
            {
                count++;
            }
            current = current->point[0];
        }

        return count;
    }

    T lower_bound(const T &key)
    {
        Node *current = head;
        T result = numeric_limits<T>::max();

        for (int i = maxlvl; i >= 0; --i)
        {
            while (current->point[i] && comp(current->point[i]->key, key))
            {
                current = current->point[i];
            }
            if (current->point[i] && !comp(current->point[i]->key, key))
            {
                result = min(result, current->point[i]->key);
            }
        }

        return (result == numeric_limits<T>::max()) ? T() : result; 
    }

    T upper_bound(const T &key)
    {
        Node *current = head;
        T result = numeric_limits<T>::max(); 

        for (int i = maxlvl; i >= 0; --i)
        {
            while (current->point[i] && comp(current->point[i]->key, key))
            {
                current = current->point[i];
            }
            if (current->point[i] && !comp(current->point[i]->key, key))
            {
                if (comp(key, current->point[i]->key))
                {
                    result = min(result, current->point[i]->key);
                }
                else
                {
                    current = current->point[i];
                    while (current->point[i] && current->point[i]->key == key)
                    {
                        current = current->point[i];
                    }
                    result = current->key;
                }
            }
        }

        if (comp(key, result))
        {
            return result;
        }

        return (result == numeric_limits<T>::max() || comp(result, key)) ? T() : current->point[0]->key; 
    }

    T closest_element(const T& key) {
        Node* current = head;
        T closest = std::numeric_limits<T>::max(); // Initialize closest with the maximum value
        bool found = false;

        for (int i = maxlvl; i >= 0; --i) {
            while (current->point[i] && comp(current->point[i]->key, key)) {
                current = current->point[i];
            }

            if (current->point[i]) {
                T next_key = current->point[i]->key;
                if (!found || std::abs(next_key - key) < std::abs(closest - key)) {
                    closest = next_key;
                    found = true;
                } else if (std::abs(next_key - key) == std::abs(closest - key)) {
                    closest = std::min(closest, next_key);
                }
            }
        }

        return found ? closest : std::numeric_limits<T>::max();
    }

    void display()
    {
        Node *current = head->point[0];
        cout << "lvl " << 0 << ": ";
        while (current)
        {
            cout << current->key << " ";
            current = current->point[0];
        }
        cout << endl;
    }
};

int main()
{
    SkipList<int> skipList;

    cout<<"Enter your choice of operations"<<endl;
    cout<<"0. Exit"<<endl;
    cout<<"1. Insertion"<<endl;
    cout<<"2. Deletion"<<endl;
    cout<<"3. Search"<<endl;
    cout<<"4. Count element occurences"<<endl;
    cout<<"5. lower_bound"<<endl;
    cout<<"6. upper_bound"<<endl;
    cout<<"7. Closest element to a value"<<endl;
    cout<<endl;

    while(1)
    {
        cout<<"Enter choice : ";

        int input;
        cin>>input;

        if(input==0)
        {
            break;
        }

        cout<<endl;
        cout<<"Enter input: ";

        int key; //here
        cin>>key;
        cout<<endl;

        if(input==1)
        {
            skipList.insert(key);
            skipList.display();
            cout<<endl;
        }
        else if(input == 2)
        {
            skipList.remove(key);
            skipList.display();
            cout<<endl;
        }
        else if(input == 3)
        {
            bool ans = skipList.search(key);

            if(ans)
            {
                cout<<"Element present"<<endl;
            }
            else
            {
                cout<<"Element not present"<<endl;
            }
            cout<<endl;
            skipList.display();
            cout<<endl;
        }
        else if(input == 4)
        {
            cout << "Number of occurence of element :"<< key<< " " << skipList.count_occurrence(key)<<endl;
            cout<<endl;
            skipList.display();
            cout<<endl;
        }
        else if(input == 5)
        {
            cout<<"Lower Bound of element :"<<key <<" "<<skipList.lower_bound(key);
            skipList.display();
            cout<<endl;
        }
        else if(input == 6)
        {
            cout<<"Upper Bound of element :"<<key <<" "<<skipList.upper_bound(key);
            skipList.display();
            cout<<endl;
        }
        else if(input == 7)
        {
            cout<<"Closest Element of element :"<<key<< " "<<skipList.closest_element(key);
            skipList.display();
            cout<<endl;
        }
    }

    return 0;
}
