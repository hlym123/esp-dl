# Tutorial: Creating a New Module(Operator)

This tutorial will guide you through the process of creating a new module in the `dl::module` namespace. The `Module` class serves as the base class for all modules, and you will be extending this base class to create your custom module.

> ATTENTION: The interface of modules in ESP-DL should be aligned with ONNX.

## Understand the Base `Module` Class

The base class provides several virtual methods that must be overridden in your derived class. 

- **Methods:**
  - `Module(const char *name, module_inplace_t inplace, quant_type_t quant_type)`: Constructor to initialize the module.
  - `~Module()`: Destructor to release resources.
  - `get_output_shape(std::vector<std::vector<int>> &input_shapes)`: Calculates the output shape based on the input shape.
  - `forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode)`: Runs the module, high-level interface.
  - `forward_args(void *args)`: Runs the module, low-level interface.
  - `deserialize(fbs::FbsModel *fbs_model, std::string node_name)`: Creates a module instance from serialized information.
  - `print()`: Prints module information.

For more information, please refer to the [Module Class Reference](../esp-dl/dl/module/include/dl_module_base.hpp).

## Create a New Module Class

To create a new module, you need to derive a new class from the `Module` base class and override the necessary methods.

### Example: Creating a `MyCustomModule` Class

For more examples, please refer to [esp-dl/dl/module](../esp-dl/dl/module/include).

```cpp
#include "module.h" // Include the header file where the Module class is defined

namespace dl {
namespace module {

class MyCustomModule : public Module {
public:
    // Constructor
    MyCustomModule(const char *name = "MyCustomModule",
                   module_inplace_t inplace = MODULE_NON_INPLACE,
                   quant_type_t quant_type = QUANT_TYPE_NONE)
        : Module(name, inplace, quant_type) {}

    // Destructor
    virtual ~MyCustomModule() {}

    // Override the get_output_shape method
    std::vector<std::vector<int>> get_output_shape(std::vector<std::vector<int>> &input_shapes) override {
        // Implement the logic to calculate the output shape based on input shapes
        std::vector<std::vector<int>> output_shapes;
        // Example: Assume the output shape is the same as the input shape
        output_shapes.push_back(input_shapes[0]);
        return output_shapes;
    }

    // Override the forward method
    void forward(std::vector<dl::TensorBase *> &tensors, runtime_mode_t mode = RUNTIME_MODE_AUTO) override {
        // Implement the logic to run the module
        // Example: Perform some operation on the tensors
        for (auto &tensor : tensors) {
            // Perform some operation on each tensor
        }
    }

    // Override the forward_args method
    void forward_args(void *args) override {
        // Implement the low-level interface logic
        // Example: Perform some operation based on the arguments
    }

    // Deserialize module instance by serialization information
    static Module *deserialize(fbs::FbsModel *fbs_model, std::string node_name){
        // Implement the logic to deserialize the module instance
        // The interface shoud be align with ONNX
    }

    // Override the print method
    void print() override {
        // Print module information
        ESP_LOGI("MyCustomModule", "Module Name: %s, Quant type: %d", name.c_str(), quant_type);
    }
};

} // namespace module
} // namespace dl
```

### Register `MyCustomModule` Class

When you have implemented `MyCustomModule` Class, please register your module in [dl_module_creator](../esp-dl/dl/module/include/dl_module_creator.hpp) as a globally available module.

```
    void register_dl_modules()
    {
        if (creators.empty()) {
            ...
            this->register_module("MyCustomModule", MyCustomModule::deserialize);
        }
    }
```