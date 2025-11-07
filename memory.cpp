#include <iostream>
#include <vector>
using namespace std;

void displayBlocks(const vector<int> &blocks)
{
    cout << "\nRemaining memory in each block:\n";
    cout << "Block\tRemaining Size\n";
    for (int i = 0; i < blocks.size(); i++)
    {
        cout << i + 1 << "\t" << blocks[i] << endl;
    }
    cout << "---------------------------------\n";
}

// ---------------- FIRST FIT ----------------
bool firstFit(vector<int> &blocks, int process, int pid)
{
    for (int j = 0; j < blocks.size(); j++)
    {
        if (blocks[j] >= process)
        {
            cout << "Process " << pid << " (Size " << process
                 << ") -> Block " << j + 1 << endl;
            blocks[j] -= process;
            return true;
        }
    }
    cout << "Process " << pid << " (Size " << process << ") -> Not Allocated\n";
    return false;
}

// ---------------- BEST FIT ----------------
bool bestFit(vector<int> &blocks, int process, int pid)
{
    int bestIdx = -1;
    for (int j = 0; j < blocks.size(); j++)
    {
        if (blocks[j] >= process)
        {
            if (bestIdx == -1 || blocks[j] < blocks[bestIdx])
                bestIdx = j;
        }
    }
    if (bestIdx != -1)
    {
        cout << "Process " << pid << " (Size " << process
             << ") -> Block " << bestIdx + 1 << endl;
        blocks[bestIdx] -= process;
        return true;
    }
    else
    {
        cout << "Process " << pid << " (Size " << process << ") -> Not Allocated\n";
        return false;
    }
}

// ---------------- WORST FIT ----------------
bool worstFit(vector<int> &blocks, int process, int pid)
{
    int worstIdx = -1;
    for (int j = 0; j < blocks.size(); j++)
    {
        if (blocks[j] >= process)
        {
            if (worstIdx == -1 || blocks[j] > blocks[worstIdx])
                worstIdx = j;
        }
    }
    if (worstIdx != -1)
    {
        cout << "Process " << pid << " (Size " << process
             << ") -> Block " << worstIdx + 1 << endl;
        blocks[worstIdx] -= process;
        return true;
    }
    else
    {
        cout << "Process " << pid << " (Size " << process << ") -> Not Allocated\n";
        return false;
    }
}

// ---------------- NEXT FIT ----------------
bool nextFit(vector<int> &blocks, int process, int pid, int &pos)
{
    int n = blocks.size();
    int count = 0;
    while (count < n)
    {
        if (blocks[pos] >= process)
        {
            cout << "Process " << pid << " (Size " << process
                 << ") -> Block " << pos + 1 << endl;
            blocks[pos] -= process;
            pos = (pos + 1) % n;
            return true;
        }
        pos = (pos + 1) % n;
        count++;
    }
    cout << "Process " << pid << " (Size " << process << ") -> Not Allocated\n";
    return false;
}

// ---------------- MAIN ----------------
int main()
{
    int nb, np;
    cout << "Enter number of memory blocks: ";
    cin >> nb;
    vector<int> blocks(nb);
    cout << "Enter size of each block:\n";
    for (int i = 0; i < nb; i++)
        cin >> blocks[i];

    cout << "Enter number of processes: ";
    cin >> np;
    vector<int> processes(np);
    cout << "Enter size of each process:\n";
    for (int i = 0; i < np; i++)
        cin >> processes[i];

    int nextPos = 0; // For next fit tracking
    cout << "\n===== MEMORY ALLOCATION (Choose strategy per process) =====\n";

    for (int i = 0; i < np; i++)
    {
        cout << "\nProcess " << i + 1 << " (Size " << processes[i] << ")\n";
        cout << "Select allocation strategy:\n";
        cout << "1. First Fit\n2. Best Fit\n3. Worst Fit\n4. Next Fit\n";
        cout << "Enter choice: ";
        int choice;
        cin >> choice;

        bool success = false;
        switch (choice)
        {
        case 1:
            success = firstFit(blocks, processes[i], i + 1);
            break;
        case 2:
            success = bestFit(blocks, processes[i], i + 1);
            break;
        case 3:
            success = worstFit(blocks, processes[i], i + 1);
            break;
        case 4:
            success = nextFit(blocks, processes[i], i + 1, nextPos);
            break;
        default:
            cout << "Invalid choice! Process skipped.\n";
        }

        displayBlocks(blocks);
        cout << "Press Enter to continue...\n";
        cin.ignore();
        cin.get();
    }

    cout << "\nAll processes completed.\n";
    return 0;
}
