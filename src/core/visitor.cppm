export module prodigy.core:visitor;

export namespace prodigy {
template <typename... Ts>
struct visitor : public Ts... {
  using Ts::operator()...;
};

template <typename... Ts>
visitor(Ts...) -> visitor<Ts...>;
}  // namespace prodigy
