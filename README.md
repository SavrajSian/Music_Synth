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

## Introduction
The synthesizer project is an embedded application designed to create, manipulate, and output audio signals. It employs a combination of hardware and software components to achieve a versatile and user-friendly experience. The synthesizer includes a range of features such as multiple waveforms, effects, volume control, and octave selection. Additionally, the project utilises CAN bus communication for data transmission and reception, enabling multiple synths to link together to form a larger keyboard.

## Features
- **Waveforms**: The synthesizer supports multiple waveforms, allowing users to choose between different sounds. These waveforms include sine, triangle, square, and sawtooth. The waveform selection is managed through a function knob, which reads the user's input and updates the waveform accordingly.

- **Effects**: The synthesizer offers various audio effects to enhance the audio output. These effects include vibrato, arpeggiator 1, and arpeggiator 2. The effects are controlled by a dedicated knob, which allows the user to select and apply the desired effect to the audio signal.

- **Volume Control:** The synthesizer provides a volume knob for adjusting the output level of the audio signal. This enables the user to control the loudness of the sound produced by the synthesizer.

- **Octave Control:** The synthesizer features an octave control system, which allows users to shift the pitch of the audio signal up or down. This is achieved through a joystick input, which reads the user's input and updates the octave selection accordingly.

- **Keyboard Input:** The synthesizer supports a 12-key input system, allowing users to play musical notes. The input is processed and sent to the synthesizer's audio generation system, which produces the corresponding audio signal based on the selected waveform, octave, and effects.

- **Display:** The project includes a display for providing visual feedback to the user. The display shows the current settings, such as volume, octave, waveform, and effects. It also indicates the current mode (CAN mode) when applicable.

- **CAN Bus Communication:** The synthesizer uses the CAN bus for data transmission and reception. This allows for communication between different devices or modules within the system. The CAN communication is managed through dedicated tasks for sending and receiving messages, with appropriate interrupts registered for efficient message handling.

- **Real-Time Control and Feedback:** The synthesizer employs a real-time operating system (RTOS) to manage tasks such as key scanning, control reading, and display updates. This ensures that the user has a responsive and seamless experience while interacting with the device.

- **Audio Generation:** The synthesizer uses a hardware timer to generate audio signals at a specified sample rate. The timer triggers an interrupt service routine (ISR), which updates the output signal based on the current waveform, pitch, and effects.
  
## Threads
The synthesizer utilises a real-time operating system (RTOS) to manage its tasks efficiently. The RTOS allows for concurrent execution of multiple tasks, ensuring a responsive user experience. This report outlines the primary threading tasks implemented in the synthesizer, along with relevant code snippets.

- **Key Scanning**  
The ```scanKeysTask``` is responsible for scanning the 12-key input system and updating the key states accordingly. This task ensures that user inputs are processed in real-time and sent to the audio generation system.

- **Control Reading**  
The ```readControlsTask``` manages the user's control inputs, such as waveform selection, effects, volume, and octave control. This task reads the user's input from knobs and a joystick, and updates the respective parameters accordingly.

- **Display**  
The ```displayKeysTask``` is responsible for updating the display with the current synthesizer settings, such as volume, octave, waveform, and effects. The task also shows the current mode (CAN mode) when applicable.

- **CAN Transmitter**  
The ```CAN_TX_Task``` handles the transmission of messages over the CAN bus. The task waits for outgoing messages in a queue and sends them over the CAN bus is a particular message format:  <br/>```[Address, Keys(1-4), Keys(5-8), Keys(9-12), Octave]```
The overall message is 5 bytes long and transmits upon new keystates.

- **CAN Receiver**  
The ```decodeTask``` is responsible for decoding incoming CAN bus messages. It processes the received messages, updating the keyboard array and octave settings accordingly.

## Performance table

| Thread Handle     | Average Iteration Time | Assigned Iteration Time | CPU Usage |
|-------------------|------------------------|-------------------------|-----------|
| ```scanKeysHandle```    |                        |                         |           |
| ```readControlsHandle```|                        |                         |           |
| ```displayKeysHandle``` |                        |                         |           |
| ```decodeTaskHandle```  |                        |                         |           |
| ```CAN_TX_TaskHandle``` |                        |                         |           |

## Inter-Task Blocking
