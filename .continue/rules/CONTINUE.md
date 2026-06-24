# CONTINUE.md - Project Guide

## Project Overview
This project is a firmware development environment for IoT devices, particularly focused on automotive and embedded systems. It utilizes C++ for implementation, with support for ESP32 and similar microcontroller platforms. The architecture is modular, with shared components and node-specific implementations.

## Getting Started
### Prerequisites
- Arduino IDE or PlatformIO
- CMake
- Python 3.x
- A compatible microcontroller board (e.g., ESP32)

### Installation Instructions
1. Clone the repository
2. Install dependencies using `platformio install`
3. Open the project in PlatformIO or Arduino IDE
4. Configure the `platformio.ini` file for your specific hardware

### Basic Usage Examples
```cpp
#include <MainNode.h>

MainNode node;

void setup() {
  node.begin();
}

void loop() {
  node.loop();
}
```

### Running Tests
- Use `platformio test` to run unit tests
- Ensure all test cases are implemented and pass before committing

## Project Structure
- **src/**: Main source files
  - **shared/**: Shared components used across all nodes
  - **node/**: Node-specific implementations
- **include/**: Header files
- **lib/**: External libraries
- **test/**: Unit tests
- **platformio.ini**: Configuration for PlatformIO
- **CMakeLists.txt**: CMake configuration
- **dependencies.lock**: Dependency lock file

## Development Workflow
### Coding Standards
- Use C++11 or later
- Follow the Google C++ Style Guide
- Use meaningful variable and function names
- Keep functions focused and single-responsibility

### Testing Approach
- Unit tests for each component
- Integration tests for node interactions
- Use `ASSERT_TRUE`, `ASSERT_FALSE`, and `TEST_F` macros

### Build and Deployment
- Use PlatformIO for build and deployment
- Flash the firmware to your microcontroller using the PlatformIO IDE
- Use `platformio run --target upload` to upload the firmware

### Contribution Guidelines
- Fork the repository
- Create a new branch for each feature or bug fix
- Write tests for any new functionality
- Follow the coding standards
- Submit a pull request

## Key Concepts
### Domain-Specific Terminology
- **Node**: A functional unit in the system
- **Event**: A signal that indicates a change in state
- **Message**: A data structure used for communication between components
- **Setting**: A configuration parameter for a node

### Core Abstractions
- **EventSystem**: Manages event registration and dispatch
- **Message**: Encapsulates data for communication
- **SettingBase**: Base class for configuration settings

### Design Patterns Used
- **Observer Pattern**: For event handling
- **Strategy Pattern**: For different node behaviors
- **Factory Pattern**: For creating different node types

## Common Tasks
### Setting Up a New Node
1. Create a new directory in `src/node/` for your node
2. Add a header file and implementation file
3. Register the node with the EventSystem
4. Implement the node's functionality
5. Write tests for the node

### Debugging Tips
- Use the `Serial.println()` function for debugging
- Use the `ESP_LOGI()` macro for logging
- Use the `platformio device monitor` command to view serial output

## Troubleshooting
### Common Issues
- **Compilation Errors**: Check for missing includes or incorrect function signatures
- **Runtime Errors**: Use the serial monitor to view error messages
- **Communication Issues**: Ensure proper wiring and correct baud rates

### Debugging Tips
- Use the `Serial.println()` function for debugging
- Use the `ESP_LOGI()` macro for logging
- Use the `platformio device monitor` command to view serial output

## References
- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32 Arduino Core Documentation](https://arduino-esp32.readthedocs.io/)
- [C++ Standard Library Documentation](https://en.cppreference.com/)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Arduino IDE Documentation](https://www.arduino.cc/reference/en/)

```