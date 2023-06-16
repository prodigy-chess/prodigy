export module prodigy.move_generator:walk;

import :node;

export namespace prodigy::move_generator {
template <Node::Context, typename Visitor>
void walk(const Node&, Visitor&&) noexcept {
  // TODO
}
}  // namespace prodigy::move_generator
