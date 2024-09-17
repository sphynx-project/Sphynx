# SphynxOS - Design

*Note: This is not finished*

## Startup process

### Bootloader

SphynxOS uses the [limine bootloader](https://limine-bootloader.org/) so check it specs for info about bootloader.

### Kernel initialization

## Style

### Logging

The logging format looks like this:
```
[time since startup] LEVEL @ Scope/Name: message
```

Example:
```
[0.000] DEBUG @ Startup/Init: Initialized ACPI using XSDT
```
