si3sim {#si3sim}
================
A tool for generating synthetic measurements of the si3 frequency.

Takes input from a config file to specify measurement schedule and other parameters.

Currently models as a constant drift rate with no measurement noise.

Output format follows that produced by saving from grafana

Entry point located in [main.cpp](\ref src/si3_sim/main.cpp)