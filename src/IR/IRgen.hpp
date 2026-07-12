#include <cstdint>
#include <optional>
#include <vector>

namespace IR {

enum class OPCODE : std::uint8_t { ADD, SUB, DIV, MULT };

struct Quadruple {
    std::optional<OPCODE> Op;
    std::optional<size_t> Src1; // source operands
    std::optional<size_t> Src2;
    size_t Dst{};               // destination of the result of: Src1 Op Src2
};

class Generator {
  private:
    std::vector<Quadruple> m_QuadList;
    size_t m_CurrRegister;

  public:
  private:
    size_t nextRegister();

    // Push a quadruple onto the list
    void emit(OPCODE op, size_t src1, size_t src2, size_t dest);
};

} // namespace IR