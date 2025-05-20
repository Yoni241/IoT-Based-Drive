# IoT-Based Distributed Storage System

## Developer: Yehonatan Peleg

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Connect-blue)](https://www.linkedin.com/in/yehonatan-peleg-33b2b2256/) 

## About Me

I'm Yoni Peleg, a software engineer with strong C/C++ skills and a passion for full-stack development. With a BSc in Information Systems, I combine theoretical knowledge with practical implementation skills to build robust, scalable systems. I'm always eager to learn new technologies and solve complex problems.

## Project Overview

This project implements a distributed storage system that leverages IoT principles to create a resilient, scalable storage solution. The system exposes a standard block device to the operating system while distributing data across multiple storage nodes for redundancy and fault tolerance.

### Key Technical Features

- **Network Block Device (NBD) Integration**: Allows the distributed storage to appear as a local block device in Linux
- **RAID-like Data Distribution**: Implements configurable data distribution strategies for fault tolerance
- **Master-Minion Architecture**: Centralized control with distributed storage execution
- **Custom Communication Protocol**: Efficient binary protocol for network communication
- **Event-Driven Design**: Non-blocking I/O operations using the reactor pattern
- **Modular Framework**: Extensible architecture with plugin support

## System Architecture

The system consists of three main components:

### Master Node
- Central controller managing the entire system
- Implements NBD protocol for OS integration
- Handles RAID-level data distribution strategies
- Routes read/write operations to appropriate minions
- Manages responses and ensures data consistency

### Minion Nodes
- Distributed storage units that persist the actual data
- Communicate with the master via UDP
- Process read/write commands independently
- Manage local storage (memory-mapped files)

### Communication Layer
- Implements messaging protocol between components
- Handles network transmission and reception
- Ensures reliable message delivery with acknowledgments
- Provides serialization and deserialization of commands

## Technical Implementation

This project demonstrates advanced C++ programming techniques including:

- Modern C++11/14 features
- Design patterns (Factory, Singleton, Command, Reactor, Thread-Pool)
- Network programming with UDP/TCP sockets
- Thread management and synchronization
- Memory-mapped file I/O
- Custom serialization protocols
- Resource management using RAII principles

## Setup and Usage

The system includes scripts for:
- Network block device setup and initialization
- Memory allocation and configuration
- Formatting and mounting the virtual disk
- System cleanup and teardown

## Project Structure

```
.
├── concrete/              # Implementation of distributed storage
│   ├── communication/     # Messaging and network protocols
│   ├── master/            # Master node implementation
│   ├── minions/           # Storage node implementations
│   └── scripts/           # System setup and management scripts
└── framework/             # Core infrastructure components
    ├── include/           # Framework headers
    └── src/               # Framework implementation
```

## Development Showcase

This project demonstrates my ability to:
- Design and implement complex distributed systems
- Write high-performance, thread-safe C++ code
- Implement network protocols and communication layers
- Integrate with operating system kernel interfaces
- Apply design patterns to solve complex problems
- Create resilient systems with fault tolerance
- Balance performance with reliability requirements

---

*This project was developed independently as a demonstration of distributed systems design and implementation skills in C++.*

