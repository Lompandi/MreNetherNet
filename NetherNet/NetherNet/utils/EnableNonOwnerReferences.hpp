/*
* Source code from:
* https://github.com/LiteLDev/LeviLamina/blob/main/src/mc/deps/core/utility/NonOwnerPointer.h
*/

#include <memory>
namespace Bedrock {
    class EnableNonOwnerReferences {
    public:
        struct ControlBlock {
        public:
            bool mIsValid;  // Indicates whether the object is still alive.
        public:
            ControlBlock& operator=(ControlBlock const&) = default;
            ControlBlock(ControlBlock const&) = default;
            ControlBlock() : mIsValid(true) {}  
        };

    public:
        std::shared_ptr<ControlBlock> mControlBlock;

    public:
        virtual ~EnableNonOwnerReferences() {
            if (mControlBlock) {
                mControlBlock->mIsValid = false;  
            }
        }

        EnableNonOwnerReferences() {
            mControlBlock = std::make_shared<ControlBlock>(); 
        }
    };

} // namespace Bedrock