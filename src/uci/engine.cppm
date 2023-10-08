export module prodigy.uci:engine;

export namespace prodigy::uci {
class Engine {
 public:
  virtual ~Engine() = default;

  virtual void stop() = 0;
};
}  // namespace prodigy::uci
