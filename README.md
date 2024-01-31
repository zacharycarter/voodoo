# voodoo
![voodoo](https://github.com/zacharycarter/voodoo/blob/master/screenshots/voodoo.png?raw=true)

voodoo is a 3D game programming library written in C.

voodoo exposes an API that is intended to be invoked via scripts written in [Janet](https://janet-lang.org/).

voodoo is focused on creating games for the browser.

voodoo is very much WIP.

## Video demonstration
[assets/scripts/game.janet](https://github.com/zacharycarter/voodoo/assets/1181513/b061174c-ac01-4f5e-84a0-370eb4942dc0)


## [Example Usage](https://github.com/zacharycarter/voodoo/blob/master/assets/scripts/game.janet)

```janet
(def state @{:camera nil})

(defn init []
  (set (state :camera) (cam/new @{:min-dist 1.0
                                  :max-dist 2000.0
                                  :center @[0.0 0.0 0.0]
                                  :distance 5.0
                                  :latitude 45.0
                                  :longitude 45.0})))

(defn event [e]
  (cam/handle-event (state :camera) e))

(defn update []
  (cam/update (state :camera))
  (dbg/draw/camera (state :camera))
  (dbg/draw/grid 0)
  (dbg/draw/cube @[0.0 0.5 0.0]))

(defn shutdown [])

(defn voodoo []
  @{:width 960
    :height 540
    :init init
    :event event
    :update update
    :shutdown shutdown})
```

## Roadmap
- [x] Memory management & Allocators
- [ ] Asset pipeline
- [ ] Build system :construction:
- [x] Virtual file system
- [x] Asset management system
- [x] Fiber-based job system
- [x] Basic orbit camera
- [x] Debug drawing
- [x] Scripting
- [ ] Visual scripting
- [ ] Forward renderer :construction:
- [ ] ImGui
- [ ] Entity component system :construction:
- [ ] Skeletal animation :construction:
- [ ] Animation controller
- [ ] Audio
- [ ] Physics
- [ ] Networking
- [x] Web (WASM) support
- [ ] Windows support
- [ ] Linux support
- [ ] MacOS support
- [ ] Android support
- [ ] iOS support

## License
voodoo is licensed under the MIT License.
