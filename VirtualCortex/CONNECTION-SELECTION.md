# Connection selection progress

Portable nearest-polyline hit testing added with deterministic first-index ties, degenerate endpoint handling and coordinate-space tolerance. Tests cover endpoints, bends, nearest selection, overlaps, misses, invalid count/tolerance and scaling. Local GCC ASan/UBSan passed (leak detection disabled). Windows CI pending.

This is a foundation only: mouse selection is NOT yet integrated. No GUI, DSP or application behavior changed; delivered 6.32 remains current. Next: share the same five-point geometry between paint and hit testing; transform mouse coordinates into design coordinates; prefer closest wire within a screen-scaled tolerance; reset selection on topology changes and slot selection; preserve row highlights and inactive dimming. Add native click tests and actual 1440p/compact screenshots before delivery. Avoid intercepting block dragging or routing-panel controls. Use guarded branch updates after refreshing heads and CI.
