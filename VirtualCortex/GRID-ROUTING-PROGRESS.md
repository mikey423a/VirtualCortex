# Grid routing development

Portable routing now provides a stereo frame entry point with parallel,
series (all four enabled rows in sequence), and dual mono (L rows 1/2,
R rows 3/4) modes. Disabled rows are bypassed. Legacy parallel APIs are
unchanged. Tests exercise physical slot identity and isolation.

Not integrated into the app or released. Required next:
- Connect the frame entry point to the live engine and saved routing selector.
- Map independent chains to selectable host/hardware channels.
- Implement block movement as a transaction carrying parameters, IR/model
  ownership and scene settings, not just the visible block type.
- Add split/merge gain/pan and migrate advanced global processors into slots.
- Native UI, processor and Windows installer validation.

Current installer remains 6.20.0.
