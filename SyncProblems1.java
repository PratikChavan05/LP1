import java.util.*;
import java.util.concurrent.*;

public class SyncProblems1 {

    static Random rand = new Random();

    static int randBet(int a, int b) {
        return a + rand.nextInt(b - a + 1);
    }

    static class ProducerConsumer{
        int BUFFER_SIZE =5;
        int [] buffer = new int [BUFFER_SIZE];
        int in=0,out=0;
        Semaphore mutex=new Semaphore(1);
        Semaphore empty = new Semaphore(BUFFER_SIZE);
        Semaphore full = new Semaphore(0);

        public void runDemo(int item){
            Thread producer=new Thread(()->{
                try {
                   for(int i=1;i<=item;i++){
                        empty.acquire();
                        mutex.acquire();
                        buffer[in] = i;
                        System.out.println("[Producer] Produced item " + i + " at pos " + in);
                        in = (in + 1) % BUFFER_SIZE;
                        mutex.release();
                        full.release();
                        Thread.sleep(randBet(54, 133));
                   }
                } catch (Exception e) {
                    System.out.println("Error in Producer");
                }
            });

            Thread consumer = new Thread(()->{
                try {
                    for(int i=0;i<=item;i++){
                        full.acquire();
                        mutex.acquire();
                        int it = buffer[out];
                        System.out.println("[Consumer] consumed item " + it + " at pos " + out);
                        out=(out+1)%BUFFER_SIZE;
                        mutex.release();
                        full.release();
                        Thread.sleep(randBet(54, 133));

                    }
                    
                } catch (Exception e) {
                    System.out.println("Error in Consumer");
                }

            });

            producer.start();
            consumer.start();
            try {
                producer.join();
                consumer.join();
            } catch (Exception e) {
                System.out.println("Error in join");
            }

            System.out.println("PC Completed");
        }
    }

    public static void main(String[] args) {
        Scanner sc =  new Scanner(System.in);
        int choice;

        choice=sc.nextInt();
        sc.nextLine();
        if(choice==1){
            new ProducerConsumer().runDemo(12);
        }else{
            System.out.println("Bye Bye!!!");
        }
        sc.close();

    }

}