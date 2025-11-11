#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h> // for sleep()
using namespace std;

class Election
{
    vector<int> processes;
    int coordinator;

public:
    void input()
    {
        int n;
        cout << "Enter number of processes: ";
        cin >> n;
        processes.resize(n);
        cout << "Enter process IDs: ";
        for (int i = 0; i < n; i++)
            cin >> processes[i];

        sort(processes.begin(), processes.end());
        coordinator = processes.back();
        cout << "\nInitial Coordinator: Process " << coordinator << "\n";
    }

    void bully()
    {
        int crash, initiator;
        cout << "The cordinator process :" << coordinator << "Crashed" << endl;
        crash = coordinator;
        cout << "Enter election initiator process ID: ";
        cin >> initiator;

        if (find(processes.begin(), processes.end(), initiator) == processes.end())
        {
            cout << "Invalid initiator!\n";
            return;
        }

        vector<int> alive;
        for (int p : processes)
            if (p != crash)
                alive.push_back(p);

        cout << "\n--- Bully Algorithm Simulation ---\n";
        cout << "Process " << initiator << " notices that coordinator (Process " << crash << ") has crashed.\n";
        sleep(1);

        cout << "\nProcess " << initiator << " sends ELECTION messages to all higher processes:\n";
        bool foundHigher = false;
        for (int p : alive)
        {
            if (p > initiator)
            {
                cout << " → Sent to Process " << p << "\n";
                sleep(1);
                foundHigher = true;
            }
        }

        if (!foundHigher)
        {
            cout << "No higher process found. Process " << initiator << " becomes new Coordinator!\n";
            coordinator = initiator;
            return;
        }

        cout << "\nHigher processes respond with OK messages...\n";
        sleep(1);

        int newCoordinator = alive.back(); // highest alive process
        cout << "\nProcess " << newCoordinator << " has the highest ID.\n";
        sleep(1);
        cout << "Process " << newCoordinator << " becomes the new Coordinator!\n";
        sleep(1);

        cout << "\nCoordinator message sent to all other processes...\n";
        for (int p : alive)
            if (p != newCoordinator)
                cout << " → Process " << p << " acknowledges new Coordinator " << newCoordinator << "\n";

        coordinator = newCoordinator;
        cout << "\nElection Complete. New Coordinator: Process " << coordinator << "\n";
    }

    void ring()
    {
        int initiator, crash;
        cout << "\nEnter election initiator process ID: ";
        cin >> initiator;
        cout << "The cordinator process :" << coordinator << "Crashed" << endl;
        crash = coordinator;

        vector<int> alive;
        for (int p : processes)
            if (p != crash)
                alive.push_back(p);

        if (find(alive.begin(), alive.end(), initiator) == alive.end())
        {
            cout << "Invalid initiator!\n";
            return;
        }

        cout << "\n--- Ring Algorithm Simulation ---\n";
        int start = find(alive.begin(), alive.end(), initiator) - alive.begin();
        vector<int> message;
        cout << "Process " << initiator << " starts election.\n";
        sleep(1);

        for (int i = 0; i < alive.size(); i++)
        {
            int current = alive[(start + i) % alive.size()];
            int next = alive[(start + i + 1) % alive.size()];
            message.push_back(current);
            cout << "Process " << current << " passes election message to Process " << next << " [";
            for (int id : message)
                cout << " " << id;
            cout << " ]\n";
            sleep(1);
        }

        cout << "\nMessage returns to initiator. Election message complete.\n";
        sleep(1);

        int newCoordinator = *max_element(message.begin(), message.end());
        cout << "Highest ID = Process " << newCoordinator << " → New Coordinator!\n";
        sleep(1);

        cout << "\nCoordinator message circulating in ring...\n";
        for (int p : alive)
            if (p != newCoordinator)
                cout << " → Process " << p << " acknowledges Coordinator " << newCoordinator << "\n";

        coordinator = newCoordinator;
        cout << "\nElection Complete. New Coordinator: Process " << coordinator << "\n";
    }

    void show()
    {
        cout << "\nCurrent Coordinator: Process " << coordinator << "\n";
    }
};

int main()
{
    Election e;
    int ch;
    cout << "\n===== ELECTION ALGORITHM SIMULATION =====\n";
    e.input();

    do
    {
        cout << "\n1. Bully Algorithm\n2. Ring Algorithm\n3. Show Coordinator\n4. Exit\n";
        cout << "Enter choice: ";
        cin >> ch;

        switch (ch)
        {
        case 1:
            e.bully();
            break;
        case 2:
            e.ring();
            break;
        case 3:
            e.show();
            break;
        case 4:
            cout << "Exiting simulation...\n";
            break;
        default:
            cout << "Invalid choice!\n";
        }
    } while (ch != 4);

    return 0;
}
