#include <iostream>
#include <vector>
#include <algorithm>
#include <unistd.h> 
using namespace std;

class Election
{
    int n;
    vector<int> processes;
    int coordinator;

public:
    void inputProcesses()
    {
        cout << "Enter number of processes: ";
        cin >> n;
        processes.resize(n);
        cout << "Enter process IDs (any order): ";
        for (int i = 0; i < n; i++)
            cin >> processes[i];

        sort(processes.begin(), processes.end());
        coordinator = processes.back();
        cout << "\nInitial Coordinator: Process " << coordinator << "\n";
    }

    void bullyAlgorithm()
    {
        int crash, initiator;
        cout << "\nEnter process ID that crashed: ";
        cin >> crash;
        cout << "Enter process ID that initiates election: ";
        cin >> initiator;

        if (find(processes.begin(), processes.end(), initiator) == processes.end())
        {
            cout << "Invalid initiator!\n";
            return;
        }

        vector<int> alive;
        for (int pid : processes)
            if (pid != crash)
                alive.push_back(pid);

        if (find(alive.begin(), alive.end(), initiator) == alive.end())
        {
            cout << "Initiator is crashed! Cannot start election.\n";
            return;
        }

        cout << "\n========== BULLY ALGORITHM SIMULATION ==========\n";
        sleep(1);

        cout << "\n[PHASE 1: Election Initiation]\n";
        cout << "Process " << initiator << " detected coordinator crash.\n";
        cout << "Process " << initiator << " starts election...\n\n";
        sleep(1);

        vector<int> higher;
        for (int pid : alive)
            if (pid > initiator)
                higher.push_back(pid);

        if (higher.empty())
        {
            cout << "No higher processes found.\n";
            cout << "Process " << initiator << " declares itself as Coordinator!\n";
            coordinator = initiator;
        }
        else
        {
            cout << "Process " << initiator << " sends ELECTION to higher processes:\n";
            for (int pid : higher)
            {
                cout << "  → ELECTION sent to Process " << pid << "\n";
                sleep(1);
            }

            cout << "\n[PHASE 2: Higher Processes Respond]\n";
            for (int pid : higher)
            {
                cout << "Process " << pid << " responds: OK to Process " << initiator << "\n";
                sleep(1);
            }

            cout << "\n[PHASE 3: Higher Processes Start Elections]\n";
            for (size_t i = 0; i < higher.size(); i++)
            {
                int current = higher[i];
                cout << "\nProcess " << current << " starts its own election:\n";

                bool hasHigher = false;
                for (size_t j = i + 1; j < higher.size(); j++)
                {
                    cout << "  → ELECTION sent to Process " << higher[j] << "\n";
                    hasHigher = true;
                    sleep(1);
                }

                if (hasHigher)
                    cout << "  Process " << current << " receives OK from higher processes\n";
                else
                    cout << "  No higher processes found!\n";
                sleep(1);
            }

            int newCoordinator = higher.back();
            cout << "\n[PHASE 4: Coordinator Announcement]\n";
            cout << "Process " << newCoordinator << " (highest ID) becomes new Coordinator!\n";
            sleep(1);
            cout << "\nProcess " << newCoordinator << " sends COORDINATOR message to all:\n";
            for (int pid : alive)
                if (pid != newCoordinator)
                    cout << "  → Process " << pid << " acknowledges new coordinator\n";

            coordinator = newCoordinator;
        }

        cout << "\n========== ELECTION COMPLETE ==========\n";
        cout << "New Coordinator: Process " << coordinator << "\n";
    }

    void ringAlgorithm()
    {
        int initiator, crash;
        cout << "\nEnter process ID initiating election: ";
        cin >> initiator;
        cout << "Enter crashed process ID (-1 if none): ";
        cin >> crash;

        vector<int> active;
        for (int pid : processes)
            if (pid != crash)
                active.push_back(pid);

        if (find(active.begin(), active.end(), initiator) == active.end())
        {
            cout << "Invalid initiator!\n";
            return;
        }

        cout << "\n========== RING ALGORITHM SIMULATION ==========\n";
        sleep(1);
        cout << "\n[PHASE 1: Election Message Circulation]\n";
        cout << "Process " << initiator << " starts election.\n";
        sleep(1);

        int start = find(active.begin(), active.end(), initiator) - active.begin();
        vector<int> electionMsg = {initiator};

        for (size_t i = 1; i < active.size(); i++)
        {
            int next = active[(start + i) % active.size()];
            electionMsg.push_back(next);
            cout << "Process " << electionMsg[i - 1] << " forwards to Process " << next << " | Message: [ ";
            for (int id : electionMsg)
                cout << id << " ";
            cout << "]\n";
            sleep(1);
        }

        cout << "\n[PHASE 2: Message Returns to Initiator]\n";
        cout << "Election message returns to Process " << initiator << "\n";
        sleep(1);

        int newCoordinator = *max_element(electionMsg.begin(), electionMsg.end());
        cout << "\n[PHASE 3: Coordinator Election]\n";
        cout << "Highest ID found = Process " << newCoordinator << "\n";
        sleep(1);

        cout << "\n[PHASE 4: Coordinator Announcement]\n";
        cout << "Process " << newCoordinator << " announces itself as Coordinator!\n";
        sleep(1);

        cout << "\nCoordinator message sent around ring:\n";
        for (int pid : active)
            if (pid != newCoordinator)
                cout << "  → Process " << pid << " acknowledges Coordinator " << newCoordinator << "\n";

        coordinator = newCoordinator;
        cout << "\n========== ELECTION COMPLETE ==========\n";
        cout << "New Coordinator: Process " << coordinator << "\n";
    }

    void displayCoordinator()
    {
        cout << "\nCurrent Coordinator: Process " << coordinator << "\n";
    }
};

int main()
{
    Election e;
    int choice;
    cout << "\n--- ELECTION ALGORITHMS SIMULATION ---\n";
    e.inputProcesses();

    do
    {
        cout << "\n1. Simulate Bully Algorithm\n2. Simulate Ring Algorithm\n3. Show Current Coordinator\n4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            e.bullyAlgorithm();
            break;
        case 2:
            e.ringAlgorithm();
            break;
        case 3:
            e.displayCoordinator();
            break;
        case 4:
            cout << "Exiting simulation...\n";
            break;
        default:
            cout << "Invalid choice!\n";
        }
    } while (choice != 4);

    return 0;
}
