# Unreal Engine Global MVVM Resolver Sample

An example implementation of the `UMVVMViewModelContextResolver` class for use as a Global Context Resolver.

This implementation is **battle-tested**, taken directly from our upcoming game [MATCHO](https://playmatcho.com), which is why all class names are prefixed with *Matcho*.

It supports:
- **Globally scoped view-models** for any view-model class.
- **Per-user scoped view-models** based on `UMatchoPerUserViewModel`.

### Usage

Set the resolver class in your project settings to `UMatchoGlobalVMResolver`, or assign it directly to a widget. Then request view-models as needed.

To access existing view-models from C++ (if they are already created), use the provided static methods:

```cpp
APlayerController* PC = ...;
if (UMyViewModel* MyVM = UMatchoViewModelsManager::FindViewModel<UMyViewModel>(PC))
{
    // Use your view-model here
}
```