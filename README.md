# ELEC60013 - SyntaxError CW2

## About
This report presents the analysis findings for the Music Synthesizer Coursework, which was completed in the Spring of 2023.

For this project, we closely followed this [specification](doc/Coursework_2_Specification.pdf), implementing further advanced features. These lab instructions were followed to create the initial foundation of our coursework:

  [Lab Part 1](doc/LabPart1.md)  
  [Lab Part 2](doc/LabPart2.md)

## Table of content
* [Introduction](./README.md#introduction)
* [Features](./README.md#features)
* [Threads](./README.md#threads)
* [Performance Table](./README.md#performance-table)
* [Inter-Task Blocking](./README.md#inter-task-blocking)
* [Atomicity](./README.md#atomicity)
* [Shared Resources](./README.md#shared-resources)
* [Task Dependencies](./README.md#task-dependencies)
## Introduction
The synthesizer project is an embedded application designed to create, manipulate, and output audio signals. It employs a combination of hardware and software components to achieve a versatile and user-friendly experience. The synthesizer includes a range of features such as multiple waveforms, effects, volume control, and octave selection. Additionally, the project utilises CAN bus communication for data transmission and reception, enabling multiple synths to link together to form a larger keyboard.

## Features
- **Waveforms**: The synthesizer supports multiple waveforms, allowing users to choose between different sounds. These waveforms include sine, triangle, square, and sawtooth. The waveform selection is managed through a function knob, which reads the user's input and updates the waveform accordingly.


- **Effects**: The synthesizer offers various audio effects to enhance the audio output. These effects include *vibrato*, *octave*, *arpeggiator 1*, *arpeggiator 2* and *chords*. The effects are controlled by a dedicated knob, which allows the user to select and apply the desired effect to the audio signal. Furthermore, the joystick acts as a pitch bender, offsetting the pitch up to 3 semi-tones above and below.  There is also a song which plays upon pressing in the 2nd knob which you can play over.

  The sine wave generation in the synthesizer is achieved using a lookup table, which provides a fast and efficient method for generating sine waves in real-time audio synthesis applications. This method allows for accurate sine wave generation while minimising computational overhead and enabling flexible control of the waveform.
  
  
  
- **Effects Control**: The synthesizer has a dedicated knob to change variables regarding the effects. This makes it easy for users to experiment with different settings and create their own unique sounds. The knob can be used to control various effect parameters such as the speed and depth of the vibrato effect, the number of octaves to shift the pitch of the sound, the pattern or style of the arpeggio effect, and the type of chords being played (major, minor, diminished, augmented, seventh). The ability to customize the effects using a single knob provides a lot of flexibility and creativity to the users, allowing them to create their own unique sound and style.


- **Volume Control:** The synthesizer provides a volume knob for adjusting the output level of the audio signal. This enables the user to control the loudness of the sound produced by the synthesizer.


- **Octave Control:** The synthesizer features an octave control system, which allows users to shift the pitch of the audio signal up or down. This is achieved through a joystick input, which reads the user's input and updates the octave selection accordingly.


- **Keyboard Input:** The synthesizer supports a 12-key input system, allowing users to play musical notes. The input is processed and sent to the synthesizer's audio generation system, which produces the corresponding audio signal based on the selected waveform, octave, and effects.


- **Display:** The project includes a display for providing visual feedback to the user. The display shows the current settings, such as volume, octave, waveform, and effects. It also indicates the current mode (CAN mode) when applicable.


- **CAN Bus Communication:** The synthesizer uses the CAN bus for data transmission and reception. This allows for communication between different devices or modules within the system. The CAN communication is managed through dedicated tasks for sending and receiving messages, with appropriate interrupts registered for efficient message handling. By hold pressing the volume knob, a new menu is displayed, allowing the user to switch CAN mode from *Master* to *Send 1* or *Send 2* with a rotation. While in *Send* mode, only the octaves are displayed.


- **Real-Time Control and Feedback:** The synthesizer employs a real-time operating system (RTOS) to manage tasks such as key scanning, control reading, and display updates. This ensures that the user has a responsive and seamless experience while interacting with the device.


- **Audio Generation:** The synthesizer uses a hardware timer to generate audio signals at a specified sample rate. The timer triggers an interrupt service routine (ISR), which updates the output signal based on the current waveform, pitch, and effects.


- **Polyphony:** The polyphony feature allows multiple notes to be played simultaneously, creating a richer and more complex sound. To efficiently manage and process these multiple notes, a *linked list* data structure is used. A linked list offers several advantages over arrays, particularly when dealing with polyphonic systems. There is a hard-set limit for polyphony, set to 84 keys at once.

  A linked list is used to store the active notes. Each node in the linked list represents an individual note and contains the step size. When a new note is played, a new node is created and added to the linked list. As notes are released or completed, their corresponding nodes are removed from the list. The ```SampleISR``` iterates through the linked list, processing each active note independently. The resulting audio samples from each note are combined (mixed) to generate the final output sound. This approach allows the synthesizer to manage and process multiple notes efficiently, even as the number of active notes changes dynamically. Memory fragmentation should not be a problem since each node is independent and can point to any location in the heap, and the chunk of memory needed for each will be small since it is just an integer and a pointer to an address; long contiguous chunks would not be needed.

  The use of linked lists allows for dynamic resizing, efficient memory usage, ease of insertion and deletion, and faster iteration of active notes, making it a suitable choice for handling polyphony in the synthesizer.
  
## Threads
The synthesizer utilises a real-time operating system (RTOS) to manage its tasks efficiently. The RTOS allows for concurrent execution of multiple tasks, ensuring a responsive user experience. This report outlines the primary threading tasks implemented in the synthesizer, along with relevant code snippets.

- **Key Scanning**  
The ```scanKeysTask``` is responsible for scanning the 12-key input system and updating the key states accordingly. This task ensures that user inputs are processed in real-time and sent to the audio generation system.

- **Control Reading**  
The ```readControlsTask``` manages the user's control inputs, such as waveform selection, effects, volume, and octave control. This task reads the user's input from knobs and a joystick, and updates the respective parameters accordingly.

- **Display**  
The ```displayKeysTask``` is responsible for updating the display with the current synthesizer settings, such as volume, octave, waveform, and effects. The task also shows the current mode (CAN mode) when applicable. The display also shows the current notes that are being pressed.

- **CAN Transmitter**  
The ```CAN_TX_Task``` handles the transmission of messages over the CAN bus. The task waits for outgoing messages in a queue and sends them over the CAN bus is a particular message format:  <br/>```[Address, Keys(1-4), Keys(5-8), Keys(9-12), Octave]```   
The overall message is 5 bytes long and transmits upon new keystates.

- **CAN Receiver**  
The ```decodeTask``` is responsible for decoding incoming CAN bus messages. It processes the received messages, updating the keyboard array and octave settings accordingly.

## Performance Table and Critical Analysis

| Thread Handle | Priority | Minimum Initiation Interval $\tau_i$ (ms) | Maximum Execution Time $T_i$ (ms) | $\lceil \frac{\tau_n}{\tau_i}\rceil T_i$ (ms)| 
|--------------------------------------------|----------|----------------------------------|-----------------------------|---------------|
| ```scanKeysHandle```                       |    5     |             20                   |       0.31                  |    1.55        |
| ```readControlsHandle```                   |    4     |             20                   |       0.52                  |    2.60       |
| ```displayKeysHandle```                    |    1     |             100                  |       17.2                  |      17.20    |
| ```decodeTaskHandle```                     |    2     |             25.2                 |       0.027                 |      0.11         |
| ```CAN_TX_TaskHandle``` & ```CAN_TX_ISR``` |    3     |             60                   |       0.91                  |       1.82        |
| ```sampleISR```                            |Interrupt |            0.045                 |      0.019                  |      42.24    |
| ```CAN_RX_ISR```                           |Interrupt |            0.7                   |       0.003                 |       0.43        |
                                                       

The worst case (maximum) execution time for  ```scanKeys``` was when all 12 keys were pressed down. ```readControls``` always does the same thing regardless of presses. ```displayKeys``` was at the worst case scenario with all keys pressed down to display on the screen. ```sampleISR``` is on a timer to interrupt 22000 times a second, so the minimum initiation interval is 1/22050 = 0.045ms. Its worst case scenario is playing a sine wave (most demanding wave) using a long list of notes (12 notes pressed). The minimum initiation interval for ```CAN_RX_ISR``` is 0.7ms since this is the minimum amount of time to transmit a CAN frame. The analysis for ```decodeTask``` is based on 36 executions with an initiation interval of 25.2ms since is has a 36 item queue that would fill in 0.7x36 = 25.2ms in the worst case. The ```CAN_TX_TaskHandle``` and ```CAN_TX_ISR``` are timed in one since the ISR needs to release the semaphore. 

The summation of the last column (latency) in the table is 65.95ms, which is less than the lowest priority task (100ms) and therefore the system performs within the timing constraints and no deadline will be missed. Even with scheduler jitter accounted for (1ms per high priority thread) it performs within the deadline.

In this worst case, the CPU is in use for 63.95% of the time, so therefore in regular usage this utilsation would be lower. For the time that the CPU is in use, the percentage of time each thread/ISR takes up is as follows:

| Thread Handle | Percentage of CPU time taken up when not idle
|---------------|--------------------------------|
|```scanKeysHandle``` | 2.35%|
|```readControlsHandle``` | 3.947%|
|```displayKeysHandle``` | 26.08%|
|| ```CAN_TX_TaskHandle``` & ```CAN_TX_ISR```|0.17%|
| ```sampleISR``` | 64.04%|                 
| ```CAN_RX_ISR``` |0.65%|   

Note: A higher number in the priority column means the task is a higher priority.

## Inter-Task Blocking
Multiple tasks run concurrently to achieve various functionalities. It is essential to manage the shared resources and communication between tasks to ensure the proper functioning of the system. Inter-task blocking can occur when one task must wait for another task to complete a specific operation, which could potentially lead to delays or even deadlocks. To avoid such issues, the following measures have been taken into account:

- **Mutex Usage**  
Mutexes have been employed in the system to protect shared resources from simultaneous access. The mutex allows only one task to access a shared resource at a time. In our synthesizer system, the ```keyArrayMutex``` is used to protect the ```keyArray``` from simultaneous access by multiple tasks. Tasks that access the ```keyArray``` must first take the mutex using ```xSemaphoreTake(keyArrayMutex, portMAX_DELAY)```. <br>
This ensures that only one task can access the shared resource at a time. Once the task has finished using the shared resource, it must release the mutex using ```xSemaphoreGive(keyArrayMutex)```.  


- **Queues**  
Queues are used for inter-task communication, providing a means to send and receive messages between tasks. In the system, two queues have been employed for CAN bus communication, msgInQ and msgOutQ. These queues store incoming and outgoing CAN bus messages:  ```msgInQ = xQueueCreate(36, 8)``` & ```  msgOutQ = xQueueCreate(36, 8)```.

  By using queues, tasks can send and receive messages without blocking each other, as they are decoupled from the sending and receiving process. When a task needs to send or receive a message, it can do so without waiting for the other tasks to complete their operations.


- **Counting Semaphores**  
A counting semaphore is used for managing the availability of resources, particularly in the CAN bus communication. In this system, the ```CAN_TX_Semaphore``` counting semaphore is employed to manage the available slots for outgoing messages on the CAN bus: ```CAN_TX_Semaphore = xSemaphoreCreateCounting(3, 3)```<br>


  When a task wants to send a message via the CAN bus, it must first take the semaphore, which represents the availability of a message slot. If there are no available slots, the task will be blocked until a slot becomes available. This mechanism ensures that the CAN bus communication is effectively managed and prevents overloading the communication channel.  
  
 ## Atomicity
 
In the synthesizer code, ```__atomic_store_n``` was used to update shared data such as control settings. By using this function, the code guarantees that other threads will not access the data while it is being updated, ensuring data consistency. For example, in the ```scanKeysTask```, a local linked list of notes is created, and the head of this list is written to the variable used by ```sampleISR``` atomically so that it is not possible for there to be an alteration of the list that ```sampleISR``` is using. The memory used by the old linked list is then freed to stop memory leaks and stop the stack size for the task being exceeded.

 ## Shared Resources
 
- **Lookup table for sine wave generation (```sinTable```)**  
The sinTable is a precomputed lookup table used for generating sine waves. It is a global resource that can be accessed by any part of the code that needs to generate sine waves.

- **Key array and octave data (```keyArray, octaveRX```)**  
The``` keyArray``` and ```octaveRX``` arrays store the current state of the synthesizer's keys and octaves. These arrays are shared between multiple tasks, such as ```scanKeysTask```, ```readControlsTask```, and ```decodeTask```. To ensure data consistency, a mutex (```keyArrayMutex```) is used to synchronise access to these shared resources.

- **Display variables (```showCAN, volume, octaveSelect, waveform, effect, canMode, canModes, effects, waves, keys```)**  
These variables are shared between the ```displayKeysTask``` and ```readControlsTask``` for displaying information on the screen. Care should be taken to avoid race conditions or inconsistent updates when modifying these variables in multiple tasks. The variables for the knobs are written to atomically to atomically for this reason.

- **CAN message variables ( ```msgInQ, msgOutQ, CAN_TX_Semaphore```)**
The incoming CAN messages recieved in ``` CAN_RX_ISR()``` are decoded in ```decodeTask``` and to ensure thread safety, a queue (```msgInQ```) is used to prevent a bottleneck in the case of an influx of recieved messages. Outgoing CAN messages (created upon note presses or releases) are stored in the ```msgOutQ``` queue in ```scanKeysTask```. The counting ```CAN_TX_Semaphore``` is used in ```CAN_TX_Task``` to regulate the outgoing messages as there are only 3 mailboxes for sending, so once the ```CAN_TX_Task``` sees there is an outgoing message and there is a free mailbox (kept track of by the semaphore), the semaphore is taken. ```CAN_TX_ISR``` gives the semaphore when a mailbox becomes free. The use of queues and a semaphore keeps the CAN functions thread safe.

## Task Dependencies
There are several tasks with dependencies between them. Identifying these dependencies is crucial to ensure correct task execution and to prevent potential issues arising from inter-task communication. Here, we discuss the dependencies between the tasks:

- ```scanKeysTask``` **and** ```decodeTask```  
  The scanKeysTask is responsible for scanning the keyboard and updating the ```keyArray``` with the current state of the keys. The ```decodeTask```, on the other hand, processes the incoming CAN messages and updates the ```keyArray``` and ```octaveRX``` accordingly. These two tasks are dependent on each other since they both modify the shared ```keyArray``` and ```octaveRX``` resources. This dependency is managed using the ```keyArrayMutex``` to synchronise access to the shared resources. However, it is not possible to get stuck in a dependency loop between two tasks requiring a mutex since one of the task will eventually unlock it.


- ```readControlsTask``` **and** ```displayKeysTask```  
The ```readControlsTask``` is responsible for reading the control inputs, such as knobs and buttons, and updating the shared display variables (```showCAN, volume, octaveSelect, waveform, effect, canMode```). The ```displayKeysTask``` is dependent on the ```readControlsTask``` since it reads these shared variables to display the information on the screen. To ensure data consistency and prevent race conditions, care should be taken when updating the shared display variables in both tasks.


- ```CAN_RX_ISR``` **and** ```decodeTask```  
The ```CAN_RX_ISR``` function is an interrupt service routine that is triggered when a new CAN message is received. It places the received message in the ```msgInQ``` queue. The ```decodeTask``` is dependent on the ```CAN_RX_ISR``` as it waits for messages in the ```msgInQ``` queue to process them. This dependency is managed using the *FreeRTOS* queue mechanism, which provides a way for tasks to wait for new messages without consuming CPU resources unnecessarily.


- ```CAN_TX_Task``` **and** ```CAN_TX_ISR```  
The ```CAN_TX_Task``` is responsible for transmitting CAN messages by dequeuing messages from the ```msgOutQ``` queue and sending them over the CAN bus. The ```CAN_TX_ISR``` function is an interrupt service routine triggered when a CAN message has been transmitted. This ISR gives a semaphore to indicate that the transmission is complete. The ```CAN_TX_Task``` is dependent on the ```CAN_TX_ISR``` to know when it can send the next message. This dependency is managed using the *FreeRTOS* semaphore mechanism, which provides a way to synchronise the execution of these tasks.

Here is the dependency graph of the tasks:

![Dependency Graph](/Dependency%20Graph.png)
