(import voodoo :prefix "")

(var world nil)
(var player nil)

(defn draw-player []
  (print "drawing player"))

(defn init []
  (set world (ecs/create-world))
  (set player (ecs/new-id world))
  (ecs/set world player Transform
           @{:pos @{:x 0.0 :y 0.0 :z 0.0}})
  (ecs/system world draw-player ecs/on-update "Transform, Renderable"))

(defn update []
  (ecs/update world))

(defn shutdown []
  (ecs/destroy-world world))

(defn voodoo []
  @{:width 100
    :height 75
    :init init
    :update update
    :shutdown shutdown})
