# Connection selection candidate

Foundation commit 6e12680718e0f5fa271be4609787a945fe97efc6 passed Windows CI 43911718896. Current candidate integrates parent-background mouse clicks, six-pixel screen-space nearest-wire tolerance, shared painted/hit polyline geometry and yellow individual highlighting (dim gray when inactive). Slot selection clears wire selection. Child block controls retain their mouse handling. DSP and presets unchanged.

NOT READY FOR DELIVERY: Windows compilation of integration pending. Add native click tests and screenshots at 2560x1440 and compact sizes; check clicks near child block bounds, overlap ties and panel controls. Complete topology/preset-change selection invalidation, then version bump from 6.32 before a user-facing release. Main remains verified 6.32; do not promote this candidate solely on the portable geometry tests. Preserve existing row focus highlights and dimming. Anubis remains user-side.
