# Producer-Consumer Project in C

This C implementation provides a solution for the producer-consumer problem in a reporter-article setting. In this scenario, reporters act as producer threads that generate articles, while the article manager serves as the dispatcher responsible for distributing articles for co-editing. The dispatcher utilizes a round-robin scheduling approach with a 5 ms quantum to ensure fair distribution. Each producer has a bounded queue for managing their articles.

## Synchronization Mechanism

The core of this implementation relies on the use of counting semaphores and binary semaphores to establish a secure synchronization mechanism between the producers and the dispatcher. This mechanism ensures that articles are produced, dispatched, and edited in a coordinated and orderly manner.

![image](https://github.com/roini7/Producer-Consumer/assets/60584742/3b7d4e50-5d76-44e7-8d9f-f2715891339e)


## Co-Editing Process

Once an article is dispatched to the co-editor thread, it is placed in an unbounded buffer, identified by its article type. The co-editor is then responsible for editing the article, and upon completion, the edited article is passed to the final queue. The main thread handles I/O operations, including printing the final articles.

## Running the Project

To run the producer-consumer project, follow these steps:

1. Compile the code by running the provided `makefile`. Simply type the following command in your terminal:

   ```
   make
   ```

2. After compilation, execute the program by specifying the path to the configuration file:

   ```
   ./ex3.out path
   ```

   - Replace `path` with the actual path to your configuration file.

## Configuration File

The configuration file should follow this format:

```
<Producer ID 1>
<Number of Articles Produced by Producer 1>
<Queue Size for Producer 1>

<Producer ID 2>
<Number of Articles Produced by Producer 2>
<Queue Size for Producer 2>
...

<Final Queue Size>
```

- Each section represents a producer and specifies:
  - The producer's ID.
  - The number of articles they produce.
  - The size of the queue available to them for storing articles.

- The final section specifies the size of the final queue where edited articles are placed.

### Example Configuration File Format

```
1
10
5

2
8
4

3
12
6

3
```

In this example, there are three producers with their respective article production counts and queue sizes, and the final queue size is 3.

Ensure there are spaces between each producer and the final queue size, as indicated above, to correctly parse the configuration file.

Feel free to modify the configuration file to match your specific requirements.

Happy producing, consuming, and co-editing!
