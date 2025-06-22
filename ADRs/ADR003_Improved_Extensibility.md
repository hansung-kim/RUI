# ADR 003: Improving Extensibility – Decoupling DisplayGUI to Support Independent Feature Additions

The current implementation of the RUI system's display functionality is centered around a monolithic DisplayGUI module. This module is responsible for handling a wide range of tasks, including:

- User interaction  
- Aircraft tracking  
- Rendering  
- Message parsing  

Such tight coupling and lack of separation of concerns make the system difficult to maintain, extend, and test.

---

## Decision

We decided to refactor the monolithic DisplayGUI module into multiple responsibility-driven modules, each dedicated to a specific concern.

### Refactored Modules

- `UIController`  
- `DisplayRenderer`  
- `UserInteractionHandler`  
- `TrackManager`  
- `MessageDecoder`  
- `ADSReceiver`  
- `FlightMetadataProvider`  
- `BigQueryUploader`

---

## Rationale

The decision is grounded in the software engineering principles of:

- High cohesion
- Low coupling

By isolating responsibilities, we achieve improved:

- Maintainability
- Extensibility
- Testability

---

## Status

**Accepted**

---

## Consequences

### ✅ Positive

- Easier to extend each component independently  
- Simplified testing and debugging  
- Reduced impact of changes when extending features or designing new functionalities

### ⚠️ Negative

- Initial **refactoring effort** and **migration cost**  
- Temporary instability due to deep DisplayGUI logic changes  
- Coordination overhead to establish and follow new module boundaries

---