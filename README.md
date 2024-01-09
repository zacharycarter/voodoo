# Voodoo

Voodoo is a 3D game programming library written in C.

Voodoo is focused on creating games for the browser.

This is very much WIP.

## Example

![assets/scripts/game.janet](https://github.com/zacharycarter/voodoo/blob/master/screenshots/ss0.png?raw=true)


```
(def state @{:camera nil})

(defn init []
  (set (state :camera) (cam/new @{:min-dist 1.0
                                  :max-dist 100.0
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



## License
Voodoo is licensed under the MIT License.