
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <chrono>
#include <random>
#include <semaphore.h>
#include <unistd.h> // sleep

using namespace std;

/* ---------- Utilities ---------- */
static std::mt19937_64 rng(std::random_device{}());
int rand_between(int a, int b)
{
    std::uniform_int_distribution<int> dist(a, b);
    return dist(rng);
}

/* ---------- 1) Producer-Consumer ---------- */
// Buffer params
const int PC_BUFFER_SIZE = 5;
int pc_buffer[PC_BUFFER_SIZE];
int pc_in = 0, pc_out = 0;

// Semaphores and mutex
sem_t pc_mutex; // binary semaphore used as mutex
sem_t pc_empty; // counting semaphore for empty slots
sem_t pc_full;  // counting semaphore for filled slots

void producer_consumer_demo(int items_to_produce = 10)
{
    // Producer
    auto producer = [items_to_produce]()
    {
        for (int i = 1; i <= items_to_produce; ++i)
        {
            int item = i;        // produce item (integer)
            sem_wait(&pc_empty); // wait for empty slot
            sem_wait(&pc_mutex); // enter critical section (binary semaphore)

            pc_buffer[pc_in] = item;
            cout << "[Producer] Produced item " << item << " at pos " << pc_in << '\n';
            pc_in = (pc_in + 1) % PC_BUFFER_SIZE;

            sem_post(&pc_mutex); // exit critical section
            sem_post(&pc_full);  // signal that one more full slot is available

            this_thread::sleep_for(chrono::milliseconds(rand_between(200, 700)));
        }
    };

    // Consumer
    auto consumer = [items_to_produce]()
    {
        for (int i = 1; i <= items_to_produce; ++i)
        {
            sem_wait(&pc_full);  // wait for an available item
            sem_wait(&pc_mutex); // enter critical section

            int item = pc_buffer[pc_out];
            cout << "  [Consumer] Consumed item " << item << " from pos " << pc_out << '\n';
            pc_out = (pc_out + 1) % PC_BUFFER_SIZE;

            sem_post(&pc_mutex); // exit critical section
            sem_post(&pc_empty); // signal empty slot available

            this_thread::sleep_for(chrono::milliseconds(rand_between(300, 900)));
        }
    };

    // initialize semaphores
    sem_init(&pc_mutex, 0, 1);
    sem_init(&pc_empty, 0, PC_BUFFER_SIZE);
    sem_init(&pc_full, 0, 0);

    thread prod(producer);
    thread cons(consumer);
    prod.join();
    cons.join();

    sem_destroy(&pc_mutex);
    sem_destroy(&pc_empty);
    sem_destroy(&pc_full);

    cout << "\nProducer-Consumer demo finished.\n\n";
}

/* ---------- 2) Readers-Writers (Readers-preference) ---------- */
sem_t rw_mutex;  // prevents writers while readers are reading (binary semaphore)
sem_t rw_mutex2; // protects read_count (binary semaphore)
int read_count = 0;
std::mutex rw_io_mutex; // for clean output

void readers_writers_demo(int num_readers = 3, int num_writers = 2, int rounds = 3)
{
    auto reader = [&](int id)
    {
        for (int r = 0; r < rounds; ++r)
        {
            // Entry section
            sem_wait(&rw_mutex2);
            read_count++;
            if (read_count == 1)
                sem_wait(&rw_mutex); // first reader locks out writers
            sem_post(&rw_mutex2);

            // Critical section (reading)
            {
                lock_guard<std::mutex> io_lock(rw_io_mutex);
                cout << "[Reader " << id << "] is reading (round " << r + 1 << "). Readers active: " << read_count << '\n';
            }
            this_thread::sleep_for(chrono::milliseconds(rand_between(200, 600)));

            // Exit section
            sem_wait(&rw_mutex2);
            read_count--;
            if (read_count == 0)
                sem_post(&rw_mutex); // last reader lets writers in
            sem_post(&rw_mutex2);

            this_thread::sleep_for(chrono::milliseconds(rand_between(300, 800)));
        }
    };

    auto writer = [&](int id)
    {
        for (int r = 0; r < rounds; ++r)
        {
            sem_wait(&rw_mutex); // wait for exclusive access
            {
                lock_guard<std::mutex> io_lock(rw_io_mutex);
                cout << "  [Writer " << id << "] is writing (round " << r + 1 << ").\n";
            }
            this_thread::sleep_for(chrono::milliseconds(rand_between(400, 900)));
            sem_post(&rw_mutex);

            this_thread::sleep_for(chrono::milliseconds(rand_between(500, 900)));
        }
    };

    sem_init(&rw_mutex, 0, 1);
    sem_init(&rw_mutex2, 0, 1);
    vector<thread> readers, writers;
    for (int i = 0; i < num_readers; ++i)
        readers.emplace_back(reader, i + 1);
    for (int i = 0; i < num_writers; ++i)
        writers.emplace_back(writer, i + 1);
    for (auto &t : readers)
        t.join();
    for (auto &t : writers)
        t.join();
    sem_destroy(&rw_mutex);
    sem_destroy(&rw_mutex2);

    cout << "\nReaders-Writers demo finished.\n\n";
}

/* ---------- 3) Dining Philosophers ---------- */
// Use semaphores as forks, plus an optional waiter semaphore (max N-1) to avoid deadlock
void dining_philosophers_demo(int n = 5, int eat_rounds = 3)
{
    vector<sem_t> forks(n);
    sem_t waiter; // allows at most n-1 philosophers to try to pick up forks concurrently
    std::mutex dp_io_mutex;

    for (int i = 0; i < n; ++i)
        sem_init(&forks[i], 0, 1);
    sem_init(&waiter, 0, n - 1); // prevents circular wait -> avoids deadlock

    auto philosopher = [&](int id)
    {
        int left = id;
        int right = (id + 1) % n;
        for (int r = 0; r < eat_rounds; ++r)
        {
            // Thinking
            {
                lock_guard<std::mutex> lk(dp_io_mutex);
                cout << "[Philosopher " << id << "] Thinking (round " << r + 1 << ")\n";
            }
            this_thread::sleep_for(chrono::milliseconds(rand_between(200, 700)));

            // Request permission from waiter
            sem_wait(&waiter);

            // pick up forks: left then right (safe because waiter restricts)
            sem_wait(&forks[left]);
            sem_wait(&forks[right]);

            {
                lock_guard<std::mutex> lk(dp_io_mutex);
                cout << "  [Philosopher " << id << "] Eating (round " << r + 1 << ")\n";
            }
            this_thread::sleep_for(chrono::milliseconds(rand_between(300, 800)));

            // put down forks
            sem_post(&forks[right]);
            sem_post(&forks[left]);

            // let waiter know finished
            sem_post(&waiter);

            this_thread::sleep_for(chrono::milliseconds(rand_between(200, 600)));
        }
    };

    vector<thread> phils;
    for (int i = 0; i < n; ++i)
        phils.emplace_back(philosopher, i);
    for (auto &t : phils)
        t.join();

    for (int i = 0; i < n; ++i)
        sem_destroy(&forks[i]);
    sem_destroy(&waiter);

    cout << "\nDining Philosophers demo finished.\n\n";
}

/* ---------- 4) Sleeping Barber ---------- */
// Semaphores: waitingCustomers (counts waiting), barberReady (barber ready to cut), chairs protected by mutex
void sleeping_barber_demo(int num_customers = 10, int num_chairs = 3)
{
    sem_t waitingCustomers; // counts waiting customers
    sem_t barberReady;      // barber signals ready to cut
    std::mutex chairs_mutex;
    int free_chairs = num_chairs;
    std::mutex sb_io_mutex;

    sem_init(&waitingCustomers, 0, 0);
    sem_init(&barberReady, 0, 0);

    // Barber thread
    auto barber = [&]()
    {
        int cuts = 0;
        while (cuts < num_customers)
        {
            // wait for a customer
            sem_wait(&waitingCustomers);

            // a customer sits -> free_chairs increases
            {
                std::lock_guard<std::mutex> lk(chairs_mutex);
                free_chairs++;
            }

            // cut hair
            {
                lock_guard<std::mutex> lk(sb_io_mutex);
                cout << "[Barber] Cutting hair. Cuts done: " << (cuts + 1) << '\n';
            }
            this_thread::sleep_for(chrono::milliseconds(rand_between(300, 800)));
            // signal customer that barber is done
            sem_post(&barberReady);
            cuts++;
        }
        // barber finishes for demo
    };

    // Customer thread
    auto customer = [&](int id)
    {
        // Arrive and try to get chair
        {
            lock_guard<std::mutex> lk(sb_io_mutex);
            cout << "[Customer " << id << "] Arrived\n";
        }
        {
            std::lock_guard<std::mutex> lk(chairs_mutex);
            if (free_chairs > 0)
            {
                free_chairs--;
                {
                    lock_guard<std::mutex> lk2(sb_io_mutex);
                    cout << "  [Customer " << id << "] Sitting. Free chairs left: " << free_chairs << '\n';
                }
                sem_post(&waitingCustomers); // notify barber
                sem_wait(&barberReady);      // wait until barber signals done
                {
                    lock_guard<std::mutex> lk2(sb_io_mutex);
                    cout << "    [Customer " << id << "] Got haircut and leaving.\n";
                }
            }
            else
            {
                // No chairs -> leaves
                lock_guard<std::mutex> lk2(sb_io_mutex);
                cout << "  [Customer " << id << "] No chairs available, leaving.\n";
            }
        }
    };

    thread barber_thread(barber);
    vector<thread> customers;
    for (int i = 1; i <= num_customers; ++i)
    {
        customers.emplace_back(customer, i);
        this_thread::sleep_for(chrono::milliseconds(rand_between(150, 400))); // stagger arrivals
    }

    for (auto &c : customers)
        c.join();
    barber_thread.join();

    sem_destroy(&waitingCustomers);
    sem_destroy(&barberReady);

    cout << "\nSleeping Barber demo finished.\n\n";
}

/* ---------- Main Menu ---------- */
int main()
{
    cout << "=== Classical Synchronization Problems (Mutex + Semaphore) ===\n";
    cout << "Select a problem to run (each runs a finite demo and exits):\n";
    cout << "1. Producer-Consumer (bounded buffer)\n";
    cout << "2. Readers-Writers (readers-preference)\n";
    cout << "3. Dining Philosophers\n";
    cout << "4. Sleeping Barber\n";
    cout << "Enter choice [1-4]: ";

    int choice;
    if (!(cin >> choice))
    {
        cerr << "Invalid input\n";
        return 1;
    }

    switch (choice)
    {
    case 1:
        cout << "\nRunning Producer-Consumer demo...\n\n";
        // init globals (positions) to 0
        pc_in = pc_out = 0;
        producer_consumer_demo(12); // produce/consume 12 items
        break;
    case 2:
        cout << "\nRunning Readers-Writers demo...\n\n";
        readers_writers_demo(4, 2, 4);
        break;
    case 3:
        cout << "\nRunning Dining Philosophers demo...\n\n";
        dining_philosophers_demo(5, 3);
        break;
    case 4:
        cout << "\nRunning Sleeping Barber demo...\n\n";
        sleeping_barber_demo(10, 3);
        break;
    default:
        cout << "Invalid choice\n";
    }

    cout << "Program finished.\n";
    return 0;
}
