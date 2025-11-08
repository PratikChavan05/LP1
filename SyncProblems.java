import java.util.concurrent.*;
import java.util.*;

public class SyncProblems {

    /* ---------- Random Utility ---------- */
    static  Random rand = new Random();

    static int randBetween(int a, int b) {
        return a + rand.nextInt(b - a + 1);
    }

    /* ---------- 1) Producer-Consumer Problem ---------- */
    static class ProducerConsumer {
        int BUFFER_SIZE = 5;
        int[] buffer = new int[BUFFER_SIZE];
        int in = 0, out = 0;

        Semaphore mutex = new Semaphore(1);
        Semaphore empty = new Semaphore(BUFFER_SIZE);
        Semaphore full = new Semaphore(0);

        public void runDemo(int itemsToProduce) {
            Thread producer = new Thread(() -> {
                try {
                    for (int i = 1; i <= itemsToProduce; i++) {
                        int item = i;
                        empty.acquire();  // wait for empty slot
                        mutex.acquire();  // enter critical section

                        buffer[in] = item;
                        System.out.println("[Producer] Produced item " + item + " at pos " + in);
                        in = (in + 1) % BUFFER_SIZE;

                        mutex.release();  // exit critical section
                        full.release();   // signal full slot

                        Thread.sleep(randBetween(200, 700));
                    }
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            });

            Thread consumer = new Thread(() -> {
                try {
                    for (int i = 1; i <= itemsToProduce; i++) {
                        full.acquire();   // wait for item
                        mutex.acquire();  // enter critical section

                        int item = buffer[out];
                        System.out.println("  [Consumer] Consumed item " + item + " from pos " + out);
                        out = (out + 1) % BUFFER_SIZE;

                        mutex.release();  // exit critical section
                        empty.release();  // signal empty slot

                        Thread.sleep(randBetween(300, 900));
                    }
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            });

            producer.start();
            consumer.start();
            try {
                producer.join();
                consumer.join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }

            System.out.println("\nProducer-Consumer demo finished.\n");
        }
    }

    /* ---------- 2) Dining Philosophers Problem ---------- */
    static class DiningPhilosophers {
        int n;
        Semaphore[] forks;
        Semaphore waiter;
        Object ioLock = new Object();

        public DiningPhilosophers(int n) {
            this.n = n;
            forks = new Semaphore[n];
            for (int i = 0; i < n; i++) forks[i] = new Semaphore(1);
            waiter = new Semaphore(n - 1); // prevent deadlock
        }

        public void runDemo(int rounds) {
            List<Thread> philosophers = new ArrayList<>();

            for (int i = 0; i < n; i++) {
                final int id = i;
                Thread phil = new Thread(() -> {
                    try {
                        for (int r = 1; r <= rounds; r++) {
                            synchronized (ioLock) {
                                System.out.println("[Philosopher " + id + "] Thinking (round " + r + ")");
                            }
                            Thread.sleep(randBetween(200, 700));

                            waiter.acquire(); // request permission

                            // pick up left and right forks
                            forks[id].acquire();
                            forks[(id + 1) % n].acquire();

                            synchronized (ioLock) {
                                System.out.println("  [Philosopher " + id + "] Eating (round " + r + ")");
                            }
                            Thread.sleep(randBetween(300, 800));

                            // put down forks
                            forks[(id + 1) % n].release();
                            forks[id].release();

                            waiter.release(); // done eating

                            Thread.sleep(randBetween(200, 600));
                        }
                    } catch (InterruptedException e) {
                        Thread.currentThread().interrupt();
                    }
                });
                philosophers.add(phil);
                phil.start();
            }

            for (Thread t : philosophers) {
                try {
                    t.join();
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                }
            }

            System.out.println("\nDining Philosophers demo finished.\n");
        }
    }

    /* ---------- Main Menu ---------- */
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        System.out.println("=== Classical Synchronization Problems (Java Semaphores) ===");
        System.out.println("1. Producer-Consumer (bounded buffer)");
        System.out.println("2. Dining Philosophers");
        System.out.print("Enter choice [1-2]: ");

        int choice = sc.nextInt();

        switch (choice) {
            case 1:
                System.out.println("\nRunning Producer-Consumer demo...\n");
                new ProducerConsumer().runDemo(12);
                break;
            case 2:
                System.out.println("\nRunning Dining Philosophers demo...\n");
                new DiningPhilosophers(5).runDemo(3);
                break;
            default:
                System.out.println("Invalid choice.");
        }

        System.out.println("Program finished.");
        sc.close();
    }
}
