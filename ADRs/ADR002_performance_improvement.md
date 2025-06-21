# ADR 002: Improve Performance of Close Control Display Time (Introduce Concurrency)

## Context

Before the application of the CPA feature, the Flight Close Control display for aircraft on the RUI met the QA-01 requirement (**response time < 1s**), with an average update time of **315.2ms over 30 attempts for 8,000 aircraft**.

However, after applying CPA, the system could only display up to around **4,000 aircraft**, and performance issues arose due to latency affecting the update time of the Close Control display.  
(Reference: *Experiment 4*)

---

## Decision

To isolate the impact of CPA computation on the RUI, a **separate thread** was created to perform only the CPA operations.  
(**Tactic:** Introduce Concurrency)

---

## Rationale

### Alternatives

| Option                          | Tactic                   | Decision | Rejection Rationale |
|---------------------------------|--------------------------|----------|----------------------|
| **Option 1**                    | Reduce Computational Overhead | ❌       | Filtering out non-essential aircraft (no location or >100 NM away) did not improve performance enough; full CPA logic rewrite was not feasible due to time constraints |
| **Option 2**                    | Introduce Concurrency    | ✅       | Concurrency separated CPA from RUI UI update, resolving performance issues without risking CPA degradation |

---

## Selection Rationale

Option 2 was selected because:

- It avoided unintended downgrades or side effects of CPA functionality.
- It separated CPA computation from RUI update processes.
- It improved both Close Control update and aircraft display performance.
- Although more complex, it effectively addressed the issue and reduced future risk.

---

## Status

**Accepted**

---

## Consequences

### ✅ Positive Outcomes

- Achieved QA-01 response goal.
- Aircraft display performed well even at 100ms RUI refresh rate (Reference: *Experiment 6*).
- Supports QA-06 (**Extensibility**) by isolating CPA thread and minimizing impact on UI.
- Easier to extend RUI functionality without disrupting CPA logic.

### ⚠️ Negative Impacts

- Slight increase in **design complexity** due to threading and structural separation.
- Minor impact on **Modifiability**, though mitigated since CPA logic is parameterized and stable.

